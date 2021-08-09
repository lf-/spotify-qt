project(spotify-qt-lib-third-party)

# SQLiteCpp
add_subdirectory(SQLiteCpp)

# Link
add_library(spotify-qt-lib-third-party STATIC)
target_link_libraries(spotify-qt-lib-third-party
	SQLiteCpp
	sqlite3)