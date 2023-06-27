/**
 * @file web_page_parse.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-06-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define DBG_TAG "WebPageParse: "
#define DEBUG 1
#define SUCCESS 0
#define FAIL (-1)

// payload (exemplo) = [{"ssid":"minharede","pass":"pwd","token":"","ip":"","mask":"","gwIp":"","gps":"0","fix":"0"}]

int8_t WebPageParse__Parse(char *str, char *result, uint8_t key_len)
{
    uint16_t str_len = strlen(str);
    uint8_t val_start_index = key_len + 4;
    uint16_t val_end_index;
    uint8_t end_found = 0;
    for (uint16_t i = val_start_index; i < str_len - 1; i++)
    {
        if (str[i] == '\\' && str[i + 1] == '\"')
        {
            str[i] = '\"';
            for (uint16_t j = i + 1; j < str_len; j++)
            {
                str[j] = str[j + 1];
            }
        }
        if (str[i] != '\\' && str[i + 1] == '\"')
        {
            end_found = 1;
            val_end_index = i + 1;
        }
        if (end_found)
        {
            break;
        }
    }
    if (!end_found)
    {
        printf(DBG_TAG "String ending index not found.\n");
        return FAIL;
    }
    strcpy(result, &str[val_start_index]);
    uint16_t last_char_index = strlen(result) - 1;
    result[val_end_index - val_start_index] = '\0';
    return SUCCESS;
}

int8_t WebPageParse__GetNum(char *payload, char *key, uint16_t *result)
{
    char result_str[16];
    uint16_t payload_len = strlen(payload);
    uint8_t key_len = strlen(key);
    if (payload_len == 0)
    {
        printf(DBG_TAG "Payload length is zero.\n");
        return FAIL;
    }
    else if (key_len == 0)
    {
        printf(DBG_TAG "Key length is zero.\n");
        return FAIL;
    }
    printf(DBG_TAG "Searching for %s value in the following payload: %s\n", key, payload);
    char key_dbl_quotes[32];
    sprintf(key_dbl_quotes, "\"%s\":\"", key);
    char *substr = strstr(payload, key_dbl_quotes);
    if (substr != NULL)
    {
        if (!WebPageParse__Parse(substr, result_str, key_len))
        {
            *result = atoi(result_str);
            printf(DBG_TAG "%s: %d\n", key, *result);
            return SUCCESS;
        }
        else
        {
            printf(DBG_TAG "Parsing failed for %s.\n", key);
            return FAIL;
        }
    }
    else
    {
        printf(DBG_TAG "No occurrences of %s in the given payload.\n", key);
        return FAIL;
    }
}

// key: "ssid":"
int8_t WebPageParse__GetStr(char *payload, char *key, char *result)
{
    uint16_t payload_len = strlen(payload);
    uint8_t key_len = strlen(key);
    if (payload_len == 0)
    {
        printf(DBG_TAG "Payload length is zero.\n");
        return FAIL;
    }
    else if (key_len == 0)
    {
        printf(DBG_TAG "Key length is zero.\n");
        return FAIL;
    }
    printf(DBG_TAG "Searching for %s value in the following payload: %s\n", key, payload);
    char key_dbl_quotes[32];
    sprintf(key_dbl_quotes, "\"%s\":\"", key);
    char *substr = strstr(payload, key_dbl_quotes);
    if (substr != NULL)
    {
        if (!WebPageParse__Parse(substr, result, key_len))
        {
            printf(DBG_TAG "%s: %s\n", key, result);
            return SUCCESS;
        }
        else
        {
            printf(DBG_TAG "Parsing failed for %s.\n", key);
            return FAIL;
        }
    }
    else
    {
        printf(DBG_TAG "No occurrences of %s in the given payload.\n", key);
        return FAIL;
    }
}

char payload[] = "[{\"ssid\":\"mi\\n\\\"ha\\\"raaa\\\"pass\\\"aaae\\\\\"de\",\"pass\":\"@#%%$_&-+<>/\\*!{}[]^~`'\\\"\",\"token\":\"\",\"ip\":\"\",\"mask\":\"\",\"gwIp\":\"\",\"gps\":\"0\",\"fix\":\"12345\"}]";

int main(void)
{
    char result_str[128];
    uint16_t result_num;
    if (WebPageParse__GetStr(payload, "ssid", result_str))
        return 1;
    if (WebPageParse__GetStr(payload, "pass", result_str))
        return 1;
    if (WebPageParse__GetNum(payload, "fix", &result_num))
        return 1;
    return 0;
}
