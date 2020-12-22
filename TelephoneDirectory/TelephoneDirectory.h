#pragma once

#include "resource.h"
#include <string>

typedef BOOL LoadBook(std::wstring* File);
LoadBook* LoadTelephoneBook;

typedef void* SearchRecord(std::wstring* surnameKey, std::wstring* streetKey, std::wstring* telephoneKey);
SearchRecord* SearchTelephoneRecord;

typedef void* getRecords();
getRecords* GetAllRecords;

struct TelephoneRecord {
	std::wstring Telephone;
	std::wstring Surname;
	std::wstring Name;
	std::wstring Patronymic;
	std::wstring Street;
	std::wstring House;
	std::wstring Building;
	std::wstring Flat;
};