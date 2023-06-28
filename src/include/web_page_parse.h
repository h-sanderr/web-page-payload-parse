/**
 * @file web_page_parse.h
 * @author Henrique Sander Lourenço
 * @brief Cabeçalho de web_page_parse.c
 * @version 0.1
 * @date 2023-06-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef WEB_PAGE_PARSE_H
#define WEB_PAGE_PARSE_H

#include <stdint.h>

int8_t WebPageParse__GetNum(char *payload, char *key, uint16_t *result);
int8_t WebPageParse__GetStr(char *payload, char *key, char *result);

#endif