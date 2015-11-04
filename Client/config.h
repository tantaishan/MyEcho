#pragma once

#include <stdio.h>
#include <iostream>

extern std::string amqp_conn_user;
extern std::string amqp_conn_pwd;
extern std::string amqp_conn_host;
extern int amqp_conn_port;

extern std::string amqp_micro_exchange;
extern std::string amqp_micro_key;

extern std::string amqp_trans_exchange;
extern std::string amqp_trans_key;

extern std::string amqp_trans_close_exchange;
extern std::string amqp_trans_close_key;


extern std::string client_config_newer_task;
extern std::string client_config_faq_web;

extern std::string login_name;
extern std::string login_pwd;

extern std::string biz_url_login;
extern std::string biz_url_helptask;

extern std::string biz_url_unfinish_task;
extern std::string biz_url_query_task;
extern std::string biz_url_feedback;
extern std::string biz_url_get_feedback_item;

extern bool config_init();
extern bool check_login();
extern bool save_login();
extern bool log_off();