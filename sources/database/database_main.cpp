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
    int rc = sqlite3_open( path, &db );
    if ( rc ) {
        fprintf( stderr, "Failed to open database %s: %s", path, sqlite3_errmsg( db ) );
        sqlite3_close( db );
        return false;
    }
    sql_execute_noresult( "PRAGMA foreign_keys = ON" );
    return true;
}

void database_main::close() {
    if ( db ) {
        sqlite3_close( db );
        db = nullptr;
    }
}

bool database_main::sql_execute_noresult(const char* sql) {
    if ( !db ) return false;

    sqlite3_stmt* stmt;
    if ( sqlite3_prepare_v2( db, sql, -1, &stmt, NULL ) != SQLITE_OK ) {
        fprintf( stderr, "sqlite3_prepare_v2 error : %s\n", sqlite3_errmsg( db ) );
        return false;
    }

    if ( sqlite3_step( stmt ) != SQLITE_DONE ) {
        fprintf( stderr, "sqlite3_step error : %s\n", sqlite3_errmsg( db ) );
        sqlite3_finalize( stmt );
        return false;
    }

    sqlite3_finalize( stmt );

    return true;
}

bool database_main::sql_check_table( const char* name ) {
    if ( !db ) return false;
    
    bool          is  = false;

    sqlite3_stmt* stmt;
    if ( sqlite3_prepare_v2( db, "SELECT name FROM sqlite_master WHERE type = 'table' AND name = (?);", -1, &stmt, NULL ) != SQLITE_OK ) {
        fprintf( stderr, "sqlite3_prepare_v2 error : %s\n", sqlite3_errmsg( db ) );
        return false;
    }

    if ( sqlite3_bind_text( stmt, 1, name, -1, SQLITE_TRANSIENT ) != SQLITE_OK ) {
        fprintf( stderr, "sqlite3_bind_text error : %s\n", sqlite3_errmsg( db ) );
        sqlite3_finalize( stmt );
        return false;
    }

    if ( sqlite3_step( stmt ) != SQLITE_ROW ) {
        fprintf( stderr, "sqlite3_prepare_v2 error : %s\n", sqlite3_errmsg( db ) );
        sqlite3_finalize( stmt );
        return false;
    }

    sqlite3_finalize( stmt );

    return true;
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