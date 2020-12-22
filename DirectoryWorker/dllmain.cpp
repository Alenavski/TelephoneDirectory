// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "TelephoneBook.h"
#include <fstream>
#include <locale>
#include <algorithm>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

Node* constructNode(TelephoneRecord* record) {
    Node* result = new Node;
    result->left = nullptr;
    result->right = nullptr;
    result->parent = nullptr;
    result->data.push_back(std::shared_ptr<TelephoneRecord>(record));
    return result;
}

Node* find(Node* tree, std::wstring key) {
    if (tree->key == key) return tree;
    if (tree->key > key) {
        return (tree->left != nullptr) ? find(tree->left, key) : tree;
    }
    else {
        return (tree->right != nullptr) ? find(tree->right, key) : tree;
    }
}

void insert(Node* tree, std::wstring key, TelephoneRecord* record) {
    Node* current = find(tree, key);
    if (current->key == key) {
        current->data.push_back(std::shared_ptr<TelephoneRecord>(record));
    }
    else if (key > current->key) {
        Node* n_node = constructNode(record);
        n_node->key = key;
        current->right = n_node;
        n_node->parent = current;
    }
    else {
        Node* n_node = constructNode(record);
        n_node->key = key;
        current->left = n_node;
        n_node->parent = current;
    }

}

bool compare(std::shared_ptr<TelephoneRecord> t1, std::shared_ptr<TelephoneRecord> t2) {
    if (t1->Surname == t2->Surname) {
        if (t1->Name == t2->Name) {
            if (t1->Patronymic == t2->Patronymic) {
                if (t1->Street == t2->Street) {
                    return t1->House < t2->House;
                } else return t1->Street < t2->Street;
            } else return t1->Patronymic < t2->Patronymic;
        } else return t1->Name < t2->Name;
    } else return t1->Surname < t2->Surname;
        
}

BOOL LoadTelephoneBook(std::wstring* File) {
    records = new std::vector<std::shared_ptr<TelephoneRecord>>;
    surnameTree = nullptr;
    streetTree = nullptr;
    telephoneTree = nullptr;
    std::wfstream in(*File);
    in.imbue(std::locale("ru_RU.utf8"));
    TelephoneRecord* record;
    while (!in.eof()) {
        record = new TelephoneRecord;
        in >> record->Surname >> record->Name >> record->Patronymic >> record->Street;
        in >> record->House >> record->Building >> record->Flat >> record->Telephone;
        if (surnameTree == nullptr) {
            surnameTree = constructNode(record);
            surnameTree->key = record->Surname;
        }
        else insert(surnameTree, record->Surname, record);
        if (streetTree == nullptr) {
            streetTree = constructNode(record);
            streetTree->key = record->Street;
        }
        else insert(streetTree, record->Street, record);
        if (telephoneTree == nullptr) {
            telephoneTree = constructNode(record);
            telephoneTree->key = record->Telephone;
        }
        else insert(telephoneTree, record->Telephone, record);
        records->push_back(std::shared_ptr<TelephoneRecord>(record));
    }
    sort(records->begin(), records->end(), compare);
    return true;
}


void* SearchTelephoneRecord(std::wstring* surnameKey, std::wstring* streetKey, std::wstring* telephoneKey) {
    Node* searchTree;
    std::wstring key;
    if (!telephoneKey->empty()) {
        searchTree = telephoneTree;
        key = *telephoneKey;
    }
    else if (!surnameKey->empty()) {
        searchTree = surnameTree;
        key = *surnameKey;
    }
    else if (!streetKey->empty()){
        searchTree = streetTree;
        key = *streetKey;
    }
    else {
        return getAllRecords();
    }
    Node* current = find(searchTree, key);
    if (current->key != key) return NULL;
    std::vector<std::shared_ptr<TelephoneRecord>> *results = new std::vector<std::shared_ptr<TelephoneRecord>>;
    for (auto record : current->data) {
        if (surnameKey->empty() || record->Surname == *surnameKey)
            if (streetKey->empty() || record->Street == *streetKey)
                if (telephoneKey->empty() || record->Telephone == *telephoneKey)
                    results->push_back(record);
    }
    sort(results->begin(), results->end(), compare);
    return results;
}

void* getAllRecords() {
    return records;
}