#include "database_header.h"

database_main database;

database_main::database_main() {  }

database_main::~database_main() { close(); }

void database_main::clear() { 
    table_tags_remove();
    table_tags_create();
    table_paths_remove();
    table_paths_create();
    table_filetypes_remove();
    table_filetypes_create();
    table_files_remove();
    table_files_create();
    table_file_set_tag_remove();
    table_file_set_tag_create();
}

bool database_main::open( const char* path ) {
    try {
        db = new SQLite::Database( path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE, 0, nullptr );
        db->exec( "PRAGMA foreign_keys = ON" );
    } catch ( std::exception& e ) {
        fprintf( stderr, "Failed to open database %s: %s", path, e.what() );
        return false;
    }
    return true;
}

void database_main::close() {
    if ( !db ) return;
    delete db;
    db = nullptr;
}

bool database_main::sql_execute_noresult(const char* sql) {
    try {
        db->exec( sql );
    } catch ( std::exception& e ) {
        fprintf( stderr, "db->exec %s: \"%s\"", e.what(), sql );
        return false;
    }
    return true;
}

bool database_main::sql_check_table( const char* name ) {
    if ( !db ) return false;
    bool is = false;
    try {
        SQLite::Statement query( *db, "SELECT name FROM sqlite_master WHERE type = 'table' AND name = (?);" );
        query.bind( 1, name );
        if ( query.tryExecuteStep() == SQLITE_ROW ) { 
            std::string getname = query.getColumn( 0 );
            is                  = (getname == name);
        }
    } catch ( std::exception& e ) {
        fprintf( stderr, "db->query %s", e.what() );
    }
    return is;
}


bool database_main::table_tags_remove() {
    if ( sql_check_table( "tags" ) == false ) return true;
    return sql_execute_noresult("DROP TABLE tags;");
}

bool database_main::table_tags_create() {
    return sql_execute_noresult("\n\
        CREATE TABLE tags\n\
        (\n\
            id			INTEGER PRIMARY KEY AUTOINCREMENT,\n\
            title		TEXT NOT NULL,\n\
            UNIQUE(title)\n\
        )\n\
        " );
}

bool database_main::table_paths_remove() {
    if ( sql_check_table( "paths" ) == false ) return true;
    return sql_execute_noresult( "DROP TABLE paths;" );
}

bool database_main::table_paths_create() {
    return sql_execute_noresult("\n\
        CREATE TABLE paths\n\
        (\n\
            id			INTEGER PRIMARY KEY AUTOINCREMENT,\n\
            path		TEXT NOT NULL,\n\
            priority    INTEGER DEFAULT(0),\n\
            UNIQUE(path)\n\
        )\n\
        " );
}

bool database_main::table_filetypes_remove() {
    if ( sql_check_table( "filetypes" ) == false ) return true;
    return sql_execute_noresult( "DROP TABLE filetypes;" );
}

bool database_main::table_filetypes_create() {
    return sql_execute_noresult( "\n\
        CREATE TABLE filetypes\n\
        (\n\
            id			INTEGER PRIMARY KEY AUTOINCREMENT,\n\
            ext		    TEXT NOT NULL,\n\
            priority    INTEGER DEFAULT(0),\n\
            miniature   TEXT,\n\
            UNIQUE(ext)\n\
        )\n\
    " );
}

bool database_main::table_files_remove() {
    if ( sql_check_table( "files" ) == false ) return true;
    return sql_execute_noresult( "DROP TABLE files;" );
}

bool database_main::table_files_create() {
    return sql_execute_noresult( "\n\
        CREATE TABLE files\n\
        (\n\
            id			    INTEGER PRIMARY KEY AUTOINCREMENT,\n\
            shortname	    NVARCHAR(128) NOT NULL,\n\
            path_to_file	TEXT NOT NULL,\n\
            path_id         INTEGER DEFAULT(0),\n\
            filetype_id     INTEGER DEFAULT(0),\n\
            filesize        INTEGER DEFAULT(0),\n\
            filetimewrite   DATETIME DEFAULT(0),\n\
            md5	            TEXT  DEFAULT(NULL),\n\
            miniature       TEXT  DEFAULT(NULL),\n\
            UNIQUE(shortname,path_to_file,path_id),\n\
            CONSTRAINT 'files_2_path_id_fk' FOREIGN KEY ('path_id') REFERENCES 'paths' ('id') ON UPDATE CASCADE ON DELETE CASCADE,\n\
            CONSTRAINT 'files_2_filetype_id_fk' FOREIGN KEY ('filetype_id') REFERENCES 'filetypes' ('id') ON UPDATE CASCADE ON DELETE CASCADE\n\
        )\n\
       " );
}

bool database_main::table_file_set_tag_remove() {
    if ( sql_check_table( "file_set_tag" ) == false ) return true;
    return sql_execute_noresult( "DROP TABLE file_set_tag;" );
}

bool database_main::table_file_set_tag_create() {
    return sql_execute_noresult( "\n\
        CREATE TABLE file_set_tag\n\
        (\n\
            file_id	        INTEGER,\n\
            tag_id	        INTEGER,\n\
            UNIQUE(file_id, tag_id),\n\
            CONSTRAINT 'file_set_tag_2_files_fk' FOREIGN KEY ('file_id') REFERENCES 'files' ('id') ON UPDATE CASCADE ON DELETE CASCADE,\
            CONSTRAINT 'file_set_tag_2_tags_fk'  FOREIGN KEY ('tag_id')  REFERENCES 'tags'  ('id') ON UPDATE CASCADE ON DELETE CASCADE\n\
        )\n\
    " );
    return true;
}

int32_t database_main::table_files_get_count() {
    if ( !db ) return 0;
    try {
        SQLite::Statement query( *db, "SELECT COUNT(*) FROM files" );
        if ( query.tryExecuteStep() == SQLITE_ROW ) {
            int32_t getcount = query.getColumn( 0 );
            return getcount;
        }
    } catch ( std::exception& e ) { fprintf( stderr, "db->query %s", e.what() ); }
    return 0;
}

#include <ctime>
std::string std_time_t_to_datetime( std::time_t t) {
    char   buffer[80];
    tm*    ltm = localtime( &t );
    strftime( buffer, sizeof( buffer ), "%Y-%m-%d %H:%M:%S", ltm );
    return std::string( buffer );
}

std::time_t datetime_to_std_time_t( const char* datatime ) {
    tm ltm = {};
    sscanf( datatime, "%d/%d/%d-%d:%d:%d", &ltm.tm_year, &ltm.tm_mon, &ltm.tm_mday, &ltm.tm_hour, &ltm.tm_min, &ltm.tm_sec );
    ltm.tm_year -= 1900;
    ltm.tm_mon--;

    std::time_t t = mktime( &ltm );
    return t;
}

bool database_main::table_files_select(uint32_t offset, uint32_t& count, database_file* buffer_out) {
    if ( !db ) return false;
    try {
        // Compile a SQL query, containing one parameter (index 1)
        SQLite::Statement query( *db, "\
            SELECT id, shortname, filetype_id, path_id, path_to_file, filesize, filetimewrite, md5, miniature \n\
            FROM files \n\
            ORDER BY id \n\
            LIMIT (?) OFFSET (?)" 
        );
        query.bind( 1, count );
        query.bind( 2, offset );

        count = 0;

        while ( query.executeStep() ) {
            database_file& ret = buffer_out[count];
            ret.id               = query.getColumn( 0 );
            ret.shortname        = query.getColumn( 1 ).getString();
            ret.filetype_id      = query.getColumn( 2 );
            ret.path_id          = query.getColumn( 3 );
            ret.path_to_file     = query.getColumn( 4 ).getString();
            ret.filesize         = (uint32_t)query.getColumn( 5 );
            ret.filetimewrite    = datetime_to_std_time_t( query.getColumn( 6 ).getText() );
            ret.md5              = query.getColumn( 7 ).getString();
            ret.name_miniature   = query.getColumn( 8 ).getString();
            count++;
        }

        return true;
    } catch ( std::exception& e ) {
        fprintf( stderr, "db->query %s", e.what() );
    }
    return false;
}
    
void database_main::testfill() {
    sql_execute_noresult( "INSERT INTO tags (title) VALUES ( 'tag1' );" );
    sql_execute_noresult( "INSERT INTO tags (title) VALUES ( 'tag2' );" );
    sql_execute_noresult( "INSERT INTO tags (title) VALUES ( 'tag3' );" );

    sql_execute_noresult( "INSERT INTO paths (path) VALUES ( 'path1' );" );
    sql_execute_noresult( "INSERT INTO paths (path, priority) VALUES ( 'path2', 11 );" );
    sql_execute_noresult( "INSERT INTO paths (path, priority) VALUES ( 'path3', 22 );" );

    sql_execute_noresult( "INSERT INTO filetypes (ext) VALUES ( 'png' );" );
    sql_execute_noresult( "INSERT INTO filetypes (ext, priority) VALUES ( 'bmp', 1 );" );
    sql_execute_noresult( "INSERT INTO filetypes (ext, priority) VALUES ( 'jpg', 2 );" );
    sql_execute_noresult( "INSERT INTO filetypes (ext, priority) VALUES ( 'mp4', 3 );" );
    sql_execute_noresult( "INSERT INTO filetypes (ext, priority) VALUES ( 'gif', 3 );" );

    sql_execute_noresult( "INSERT INTO files (shortname, path_to_file, path_id, filetype_id) VALUES ( 'file1.png', 'dirA/file1.png', 1, 1 );" );
    sql_execute_noresult( "INSERT INTO files (shortname, path_to_file, path_id, filetype_id) VALUES ( 'file2.png', 'dirB/file2.png', 2, 1 );" );
    sql_execute_noresult( "INSERT INTO files (shortname, path_to_file, path_id, filetype_id) VALUES ( 'file3.png', 'dirC/file3.png', 2, 2 );" );

    sql_execute_noresult( "INSERT INTO file_set_tag (file_id, tag_id) VALUES ( 1, 1 );" );
    sql_execute_noresult( "INSERT INTO file_set_tag (file_id, tag_id) VALUES ( 1, 2 );" );
    sql_execute_noresult( "INSERT INTO file_set_tag (file_id, tag_id) VALUES ( 3, 2 );" );
    sql_execute_noresult( "INSERT INTO file_set_tag (file_id, tag_id) VALUES ( 3, 3 );" );
    sql_execute_noresult( "INSERT INTO file_set_tag (file_id, tag_id) VALUES ( 2, 1 );" );
    
    // SELECT * FROM files
    // SELECT id,shortname FROM files
    // SELECT * FROM files WHERE path_id = 1;
    // DELETE FROM files WHERE id=3;
    // DELETE FROM file_set_tag WHERE file_id=3;
}




/*

    // for multiline
    //int rc;
    //while ( ( rc = sqlite3_step( stmt ) ) == SQLITE_ROW ) {
    //    is = true;
    //    break;
    //}
    //if ( rc != SQLITE_DONE ) {
    //    fprintf( stderr, "sqlite3_prepare_v2 error : %s\n", sqlite3_errmsg( db ) );
    //    sqlite3_finalize( stmt );
    //}


 sqlite3_stmt* stmt;
    sqlite3_prepare_v2( db, sql.c_str(), -1, &stmt, NULL );
    while ( sqlite3_step( stmt ) != SQLITE_DONE ) {
        int i;
        int num_cols = sqlite3_column_count( stmt );

        for ( i = 0; i < num_cols; i++ ) {
            switch ( sqlite3_column_type( stmt, i ) ) {
                case ( SQLITE3_TEXT ): printf( "%s, ", sqlite3_column_text( stmt, i ) ); break;
                case ( SQLITE_INTEGER ): printf( "%d, ", sqlite3_column_int( stmt, i ) ); break;
                case ( SQLITE_FLOAT ): printf( "%g, ", sqlite3_column_double( stmt, i ) ); break;
                default: break;
            }
        }
    }
    sqlite3_finalize( stmt );
*/