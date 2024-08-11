/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

// 包含 "OpenAI.h" 头文件
#include "OpenAI.h"

// SSID 的大小
#define SSID_SIZE 32
// Password 的大小
#define PASSWORD_SIZE 64
// KEY 的大小
#define KEY_SIZE 64
// URL 的大小
#define URL_SIZE 64

// 定义一个结构体，用于存储系统参数
typedef struct {
    char ssid[SSID_SIZE];                  //* 目标接入点的SSID。 
    char password[PASSWORD_SIZE];          //* 目标接入点的密码。
    char key_chat_api[KEY_SIZE];          //* AI密钥。 
    char key_chat_secret[KEY_SIZE];       //* AI密钥。 
    char url_chat[URL_SIZE];                //* AI基础URL。 
    char key_voice_api[KEY_SIZE];         //* 语音识别密钥。 
    char key_voice_secret[KEY_SIZE];      //* 语音识别密钥。
    char url_voice_tts[URL_SIZE];         //* tts基础URL。
    char url_voice_stt[URL_SIZE];         //* stt基础URL。
} sys_param_t;

// 进行工厂重置
esp_err_t settings_factory_reset(void);
// 从 NVS 中读取参数
esp_err_t settings_read_parameter_from_nvs(void);
// 获取参数
sys_param_t *settings_get_parameter(void);