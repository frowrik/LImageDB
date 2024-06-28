#pragma once

// app base
#include "header.h"

// database
#include <sqlite3.h>

// �������� ����
struct database_tag {
    int64_t     id;     // ���������� ��
    std::string title;  // ����� 
};

// �������� ����
struct database_path {
    int64_t     id;        // ���������� �� 
    std::string path;      // ���� �� ����� 
    int32_t     priority;  // ��������� ����� (��������� ����� ������ ���� ������� �������� ����� �������)
};

// �������� ���� �����
struct database_filetype {
    int64_t     id;              // ���������� �� 
    std::string ext;             // ���������� �����, ������ ".bmp"
    int32_t     priority;        // ��������� ���� ����� (������ jpg ����� ����������� � ��� ������ ������ ��������)
    std::string name_miniature;  // ��� ��� ��������� ���� �����
};

// �������� ����� (����������� � ����)
struct database_file {
public:
    int64_t     id;              // ���������� ��
    std::string shortname;       // �������� ��� ������������� �� ��������� ����� ����
    int64_t     filetype_id;     // ��� �����
    int64_t     path_id;         // �� ������� ����� ��� �����������
    std::string path_to_file;    // ���� �� ����������� ��� ����� ������� �����
    size_t      filesize;        // ������ ����� �� �����
    std::time_t filetimewrite;   // ����� ���� ��������������
    std::string md5;             // ����� ����� ��� ������ ����������
    std::string name_miniature;  // ��� ��� ��������������� ���������
};

// ����������� ����� � ������
struct database_file_tags {
    int64_t file_id;  //
    int64_t tag_id;   //
};

// ��� �������� ������ ����������� ��� ���������
struct database_sound {
    int64_t     file_id;  // ����� � ������ 
    double      time;     // ������������
    double      bpm;      // ����
    std::string key;      // �����������
    std::string genre;    // ����
};

class database_main {
public:
    database_main();
    ~database_main();

    bool open( const char* path );
    void close( );

    void clear();
    void testfill();

public:
    bool table_tags_remove();
    bool table_tags_create();
    bool table_paths_remove();
    bool table_paths_create();
    bool table_filetypes_remove();
    bool table_filetypes_create();
    bool table_files_remove();
    bool table_files_create();
    bool table_file_set_tag_remove();
    bool table_file_set_tag_create();

public:
    int32_t table_files_get_count();
    bool    table_files_select( uint32_t offset, uint32_t& count, database_file* buffer_out );

public:
    sqlite3* db = nullptr;
    
    bool sql_check_table(const char* name);
    bool sql_execute_noresult(const char* sql);
};
extern database_main database;