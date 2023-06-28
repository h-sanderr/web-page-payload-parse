/**
 * @file web_page_parse.c
 * @author Henrique Sander Lourenço
 * @brief Funções para interpretar dados de página web em formato JSON.
 * @version 0.1
 * @date 2023-06-27
 *
 * @copyright Copyright (c) I-SENSI 2023
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define WEB_PAGE_PARSE_TAG "WebPageParse: "
#define WEB_PAGE_PARSE_OK 0
#define WEB_PAGE_PARSE_FAIL (-1)
#define WEB_PAGE_PARSE_TEST 0

// payload (exemplo) = [{"ssid":"minharede","pass":"pwd","token":"","ip":"","mask":"","gwIp":"","gps":"0","fix":"0"}]

/**
 * @brief Interpreta a string \p str contendo o valor de uma chave de comprimento \p key_len e guarda
 * o valor associado à esta chave em \p result.
 *
 * @param str String começando na posição das primeiras aspas duplas da chave cujo valor deseja-se
 * encontrar. Por exemplo, se o objetivo é saber o valor da chave "ssid", deve-se passar uma string
 * na forma: <"ssid":"meu ssid",...}> (os caracteres '<' e '>' são penas para delimitar a string,
 * não fazem parte da string passada como argumento). Não deve haver espaços separando os dois
 * pontos (':') e as aspas duplas ('"').
 * @param result Ponteiro apontando para o endereço onde o resultado será guardado.
 * @param key_len Comprimento da chave. Por exemplo, a chave ssid tem comprimento 4.
 * @return int8_t
 * @retval 0 se sucesso.
 * @retval -1 se falha.
 */
static int8_t WebPageParse__Parse(char *str, char *result, uint8_t key_len)
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
        printf(WEB_PAGE_PARSE_TAG "String ending index not found.\n");
        return WEB_PAGE_PARSE_FAIL;
    }
    strcpy(result, &str[val_start_index]);
    uint16_t last_char_index = strlen(result) - 1;
    result[val_end_index - val_start_index] = '\0';
    return WEB_PAGE_PARSE_OK;
}

int8_t WebPageParse__GetStr(char *payload, char *key, char *result);

/**
 * @brief Interpreta dados recebidos da página web de configuração do gateway encontra valor
 * de inteiro associado a determinada chave.
 *
 * @param payload Dados recebidos da página web. Aspas duplas no meio de um valor devem ser
 * precedidas por uma barra invertida ('\'). Não deve haver espaços entre as aspas duplas
 * ('"') delimitadoras de chave/valor e os dois pontos (':') separando a chave do valor.
 * @param key Chave da qual se deseja encontrar o valor.
 * @param result Ponteiro apontando para o endereço onde o resultado será armazenado.
 * @return int8_t
 * @retval 0 se sucesso.
 * @retval -1 se falha.
 */
int8_t WebPageParse__GetNum(char *payload, char *key, uint16_t *result)
{
    char result_str[16];
    if (!WebPageParse__GetStr(payload, key, result_str))
    {
        *result = atoi(result_str);
        if (*result == 0)
            printf(WEB_PAGE_PARSE_TAG "atoi() returned zero, this could mean that conversion failed or it's actually zero.");
        printf(WEB_PAGE_PARSE_TAG "Integer result for %s: %d\n", key, *result);
        return WEB_PAGE_PARSE_OK;
    }
    else
    {
        printf(WEB_PAGE_PARSE_TAG "Parsing failed for %s.\n", key);
        return WEB_PAGE_PARSE_FAIL;
    }
}

/**
 * @brief Interpreta dados recebidos da página web de configuração do gateway encontra valor
 * de string associado a determinada chave.
 *
 * @param payload Dados recebidos da página web. Aspas duplas no meio de um valor devem ser
 * precedidas por uma barra invertida ('\'). Não deve haver espaços entre as aspas duplas
 * ('"') delimitadoras de chave/valor e os dois pontos (':') separando a chave do valor.
 * @param key Chave da qual se deseja encontrar o valor.
 * @param result Ponteiro apontando para o endereço onde o resultado será armazenado.
 * @return int8_t
 * @retval 0 se sucesso.
 * @retval -1 se falha.
 */
int8_t WebPageParse__GetStr(char *payload, char *key, char *result)
{
    uint16_t payload_len = strlen(payload);
    uint8_t key_len = strlen(key);
    if (payload_len == 0)
    {
        printf(WEB_PAGE_PARSE_TAG "Payload length is zero.\n");
        return WEB_PAGE_PARSE_FAIL;
    }
    else if (key_len == 0)
    {
        printf(WEB_PAGE_PARSE_TAG "Key length is zero.\n");
        return WEB_PAGE_PARSE_FAIL;
    }
    printf(WEB_PAGE_PARSE_TAG "Searching for %s value in the following payload: %s\n", key, payload);
    char key_dbl_quotes[32];
    sprintf(key_dbl_quotes, "\"%s\":\"", key);
    char *substr = strstr(payload, key_dbl_quotes);
    if (substr != NULL)
    {
        if (!WebPageParse__Parse(substr, result, key_len))
        {
            printf(WEB_PAGE_PARSE_TAG "String result for %s: %s\n", key, result);
            return WEB_PAGE_PARSE_OK;
        }
        else
        {
            printf(WEB_PAGE_PARSE_TAG "Parsing failed for %s.\n", key);
            return WEB_PAGE_PARSE_FAIL;
        }
    }
    else
    {
        printf(WEB_PAGE_PARSE_TAG "No occurrences of %s in the given payload.\n", key);
        return WEB_PAGE_PARSE_FAIL;
    }
}

#if WEB_PAGE_PARSE_TEST
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
#endif