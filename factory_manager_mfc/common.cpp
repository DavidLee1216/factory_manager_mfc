#include "pch.h"
#include "factory_manager_mfc.h"
#include <string.h>
#include <string>
#include <mysql.h>

using namespace std;

extern MYSQL* mysql;

char* CStringToChar(CString str, char* charStr)
{
	int len = str.GetLength();
	WideCharToMultiByte(CP_ACP, 0, str, -1, charStr, len, NULL, NULL);
	charStr[len] = 0;
	return charStr;
}

char* TCHARToChar(TCHAR* str, char* charStr)
{
	int len = _tcslen(str);
	WideCharToMultiByte(CP_ACP, 0, str, -1, charStr, len, NULL, NULL);
	charStr[len] = 0;
	return charStr;
}

int CStringToInt(CString str)
{
	char ss[20];
	CStringToChar(str, ss);
	return atoi(ss);
}

double CStringToDouble(CString str)
{
	char ss[20];
	CStringToChar(str, ss);
	return atof(ss);
}

void SplitDate(CString str, int arr[3])
{
	int nTokenPos = 0;
	CString strToken = str.Tokenize(_T("-"), nTokenPos);
	int i = 0;
	while (!strToken.IsEmpty())
	{
		arr[i] = CStringToInt(strToken);
		strToken = str.Tokenize(_T("-"), nTokenPos);
		i++;
	}
}

TCHAR* CharToTCHAR(char* charStr, TCHAR* str)
{
	int len = strlen(charStr);
	MultiByteToWideChar(CP_ACP, 0, charStr, len, str, len);
	str[len] = 0;
	return str;
}

void trim(char* str)
{
	int len = strlen(str);
	while (str[len - 1] == ' ' || str[len - 1] == '\r' || str[len - 1] == '\n')
	{
		str[len - 1] = 0;
		len--;
	}
	int start = 0;
	while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
	{
		start++;
	}
	memmove(str, str + start, len - start);
}

string GbkToUtf8(const char* src_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

void Utf8ToGbk(const char* src_str, char *new_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	strcpy_s(new_str, 100, szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
}

wchar_t* Utf8_2_Unicode(char* row_i, wchar_t* wszStr)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, row_i, strlen(row_i), NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, row_i, strlen(row_i), wszStr, len);
	wszStr[len] = '\0';
	return wszStr;
}

char* Unicode_2_Utf8(wchar_t* wszStr, char* charStr)
{
	if (wcslen(wszStr) == 0) {
		charStr[0] = 0;
		return ""; // nothing to do
	}
	CStringA utf8;
	int cc = 0;
	if ((cc = WideCharToMultiByte(CP_UTF8, 0, wszStr, -1, NULL, 0, 0, 0) - 1) > 0)
	{
		WideCharToMultiByte(CP_UTF8, 0, wszStr, -1, charStr, cc, 0, 0);
		charStr[cc] = 0;
		return charStr;
	}
	charStr[0] = 0;
	return "";
}

int get_back_count(int id)
{
	char query[150];
	int cnt = 0;
	sprintf_s(query, "select sum(count) from back_info where asset_id='%d'", id);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			if(row[0] != 0)
				cnt = atoi(row[0]);
		}
		mysql_free_result(res);
	}
	return cnt;
}

int get_exhaust_count(int id)
{
	char query[150];
	int cnt = 0;
	sprintf_s(query, "select sum(count) from exhaust_info where asset_id='%d'", id);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			if (row[0] != 0)
				cnt = atoi(row[0]);
		}
		mysql_free_result(res);
	}
	return cnt;
}

int get_total_placed_count()
{
	char query[150];
	int cnt = 0;
	sprintf_s(query, "select sum(count) from placement_info");
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			if (row[0] != 0)
				cnt = atoi(row[0]);
		}
		mysql_free_result(res);
	}
	return cnt;
}
int get_placed_count(int id)
{
	char query[150];
	int cnt = 0;
	sprintf_s(query, "select sum(count) from placement_info where asset_id='%d'", id);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			if (row[0] != 0)
				cnt = atoi(row[0]);
		}
		mysql_free_result(res);
	}
	return cnt;
}

int get_asset_count(int id)
{
	char query[150];
	int cnt = 0;
	sprintf_s(query, "select count from assets_info where id='%d'", id);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			if (row[0] != 0)
				cnt = atoi(row[0]);
		}
		mysql_free_result(res);
	}
	return cnt;
}

int get_remain_asset_count(int asset_id) {
	int back_cnt = get_back_count(asset_id);
	int exhaust_cnt = get_exhaust_count(asset_id);
	int placed_cnt = get_placed_count(asset_id);
	int asset_cnt = get_asset_count(asset_id);
	int remainCount = asset_cnt - back_cnt - exhaust_cnt - placed_cnt;
	return remainCount;
}

int get_last_id_from_table(char* tb) {
	char query[100];
	int id = 0;
	sprintf_s(query, "select id from %s order by id desc", tb);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count == 0)
			id = 1;
		else {
			MYSQL_ROW row = mysql_fetch_row(res);
			id = atoi(row[0]) + 1;
		}
		mysql_free_result(res);
	}
	return id;
}

bool check_user_pass(char* username, char* password) {
	char query[100];
	char pass[50];
	pass[0] = 0;
	sprintf_s(query, "select password from user_info where username='%s'", username);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			strcpy_s(pass, row[0]);
		}
		else {
			mysql_free_result(res);
			return false;
		}
		mysql_free_result(res);
	}
	return strcmp(pass, password)==0;
}

bool check_user(char* username) {
	char query[100];
	bool b = false;
	sprintf_s(query, "select * from user_info where username='%s'", username);
	if (mysql_query(mysql, query) == 0) {
		MYSQL_RES* res = mysql_store_result(mysql);
		if (res->row_count > 0)
			b = true;
		mysql_free_result(res);
	}
	return b;
}

bool set_pass(char* username, char* password) {
	char query[200];
	bool b = false;
	sprintf_s(query, "update user_info set password='%s' where username='%s'", password, username);
	if (mysql_query(mysql, query) == 0) {
		b = true;
	}
	return b;
}

bool modify_user(char* old_user, char* new_user) {
	char query[200];
	bool b = false;
	sprintf_s(query, "update user_info set username='%s' where username='%s'", new_user, old_user);
	if (mysql_query(mysql, query) == 0) {
		b = true;
	}
	return b;
}

bool delete_user(char* user) {
	char query[100];
	bool b = false;
	sprintf_s(query, "delete from user_info where username='%s'", user);
	if (mysql_query(mysql, query) == 0) {
		b = true;
	}
	return b;
}