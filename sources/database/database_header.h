#pragma once

// app base
#include "header.h"

// database
#include <sqlite3.h>

// описание тега
struct database_tag {
    int64_t     id;     // уникальный ид
    std::string title;  // текст 
};

// описание тега
struct database_path {
    int64_t     id;        // уникальный ид 
    std::string path;      // путь до папки 
    int32_t     priority;  // приоритет папки (некоторые папки должны быть главней например папка артиста)
};

// описание типа файла
struct database_filetype {
    int64_t     id;              // уникальный ид 
    std::string ext;             // расширение файла, пример ".bmp"
    int32_t     priority;        // приоритет типа файла (пример jpg менее качесвенный и для поиска важней оригинал)
    std::string name_miniature;  // имя для миниатюры типа файла
};

// описание файла (изображения и звук)
struct database_file {
public:
    int64_t     id;              // уникальный ид
    std::string shortname;       // короткое имя сгенерировано из последней части пути
    int64_t     filetype_id;     // тип файла
    int64_t     path_id;         // ид базовой папки где изображение
    std::string path_to_file;    // путь до изображения без учета базовой папки
    size_t      filesize;        // размер файла на диске
    std::time_t filetimewrite;   // когда файл редактировался
    std::string md5;             // сумма файла для поиска дубликатов
    std::string name_miniature;  // имя для сгенерированной миниатюры
};

// подключение тегов к файлам
struct database_file_tags {
    int64_t file_id;  //
    int64_t tag_id;   //
};

// для описания звуков испозуються доп структура
struct database_sound {
    int64_t     file_id;  // связь с файлом 
    double      time;     // длительность
    double      bpm;      // темп
    std::string key;      // тональность
    std::string genre;    // жанр
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