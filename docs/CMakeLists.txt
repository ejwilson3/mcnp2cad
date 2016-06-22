find_package(Sphinx REQUIRED)


# configured documentation tools and intermediate build results
set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/_build")
 
# Sphinx cache with pickled ReST documents
set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_doctrees")
 
# HTML output directory
set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")
   
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in"
    "${BINARY_BUILD_DIR}/conf.py"
    @ONLY)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/_themes/" DESTINATION "${BINARY_BUILD_DIR}/_themes/")
                
add_custom_target(mcnp2cad_docs ALL
    ${SPHINX_EXECUTABLE}
    	-q -b html
    	-c "${BINARY_BUILD_DIR}"
    	-d "${SPHINX_CACHE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}"
	    "${SPHINX_HTML_DIR}"
    COMMENT "Building HTML documentation with Sphinx")
