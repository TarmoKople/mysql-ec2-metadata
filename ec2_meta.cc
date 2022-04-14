#include <ctype.h>
#include <mysql/plugin.h>
#include <mysql/service_my_plugin_log.h>
#include <sql/sql_class.h>
#include <sql/sql_show.h>
#include <sql/table.h>
#include <stdlib.h>
#include <regex>
#include <string>
#include <vector>
#include "HTTPRequest.hpp"

static MYSQL_PLUGIN plugin_info_ptr;

std::string ec2_meta_keys[] = {"placement/availability-zone",
                               "placement/availability-zone-id",
                               "placement/region",
                               "ami-id",
                               "ami-launch-index",
                               "ami-manifest-path",
                               "hostname",
                               "instance-action",
                               "instance-id",
                               "instance-life-cycle",
                               "instance-type",
                               "local-hostname",
                               "local-ipv4",
                               "mac",
                               "profile",
                               "reservation-id",
                               "security-groups"};

std::string ec2_meta_url = "http://169.254.169.254/latest/meta-data/";

struct meta_row_t {
  std::string key;
  std::string value;
};
static std::vector<meta_row_t> ec2_meta_rows;
static bool ec2_meta_downloaded = false;

static struct st_mysql_information_schema ec2_meta_table_info = {
    MYSQL_INFORMATION_SCHEMA_INTERFACE_VERSION};

static ST_FIELD_INFO ec2_meta_table_fields[] = {
    {"METAOPT", 255, MYSQL_TYPE_STRING, 0, 0, 0, 0},
    {"VALUE", 255, MYSQL_TYPE_STRING, 0, 0, 0, 0},
    {0, 0, MYSQL_TYPE_NULL, 0, 0, 0, 0}};

static int ec2_meta_fill_table(THD *thd, TABLE_LIST *tables, Item *cond) {
  std::ignore = cond;


  if (!ec2_meta_downloaded) {
    my_plugin_log_message(&plugin_info_ptr, MY_INFORMATION_LEVEL,
                          "downloading ec2 meta info...");
    for (uint i = 0; i < sizeof(ec2_meta_keys) / sizeof(*ec2_meta_keys); i++) {
      try {
        http::Request request{ec2_meta_url + ec2_meta_keys[i],
                              http::InternetProtocol::V4};
        const auto response =
            request.send("GET", "",
                         {{"Content-Type", "application/x-www-form-urlencoded"},
                          {"User-Agent", "mysql-ec2-meta/0.1"},
                          {"Accept", "*/*"}},
                         std::chrono::milliseconds (250));
        if (response.status.code == http::Status::Ok) {
          std::string res =
              std::string{response.body.begin(), response.body.end()};
          res = std::regex_replace(res, std::regex("\\r$|\\n$"), "");

          ec2_meta_rows.push_back(meta_row_t{ec2_meta_keys[i], res});
          my_plugin_log_message(&plugin_info_ptr, MY_INFORMATION_LEVEL,
                                "got info %s", ec2_meta_keys[i].c_str());
        } else {
          continue;
        }
      } catch (const http::RequestError &e) {
        my_plugin_log_message(&plugin_info_ptr, MY_ERROR_LEVEL,
                              "request error: %s", e.what());
      } catch (const http::ResponseError &e) {
        my_plugin_log_message(&plugin_info_ptr, MY_ERROR_LEVEL,
                              "response error: %s", e.what());

      } catch (const std::exception &e) {
        my_plugin_log_message(&plugin_info_ptr, MY_ERROR_LEVEL, "error: %s",
                              e.what());
      }
    }
    my_plugin_log_message(&plugin_info_ptr, MY_INFORMATION_LEVEL, "done.");
    ec2_meta_downloaded = true;
  }

  TABLE *table = tables->table;

  std::vector<meta_row_t>::iterator iter;
  for (iter = ec2_meta_rows.begin(); iter != ec2_meta_rows.end(); ++iter) {
    table->field[0]->store(iter->key.c_str(), iter->key.length(),
                           system_charset_info);
    table->field[1]->store(iter->value.c_str(), iter->value.length(),
                           system_charset_info);
    if (schema_table_store_record(thd, table)) return 1;
  }
  return 0;
}

static int ec2_meta_table_init(MYSQL_PLUGIN plugin_info) {
  plugin_info_ptr = plugin_info;

  ST_SCHEMA_TABLE *schema_table = (ST_SCHEMA_TABLE *)plugin_info;

  schema_table->fields_info = ec2_meta_table_fields;
  schema_table->fill_table = ec2_meta_fill_table;
  return 0;
}

static int ec2_meta_table_deinit(MYSQL_PLUGIN plugin_info) {
  std::ignore = plugin_info;
  ec2_meta_downloaded = false;
  ec2_meta_rows.clear();
  return 0;
}

mysql_declare_plugin(EC2_META){
    MYSQL_INFORMATION_SCHEMA_PLUGIN,
    &ec2_meta_table_info,                     /* type-specific descriptor */
    "EC2_META",                               /* table name */
    "Tarmo Kople",                            /* author */
    "Exposes EC2 instance metadata as table", /* description */
    PLUGIN_LICENSE_GPL,                       /* license type */
    ec2_meta_table_init,                      /* init function */
    ec2_meta_table_deinit,
    NULL,
    0x0100, /* version = 1.0 */
    NULL,   /* no status variables */
    NULL,   /* no system variables */
    NULL,   /* no reserved information */
    0       /* no flags */
} mysql_declare_plugin_end;
