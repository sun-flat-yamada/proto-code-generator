/**
 * @file
 *
 * @author sun-flat-yamada
 * @brief
 * @version 0.1
 * @date 2020-08-23
 *
 * @see   https://pantor.github.io/inja/
 * *
 * @copyright Copyright (c) 2020
 *
 */
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "single_include/inja/inja.hpp"
#include "single_include/nlohmann/json.hpp"
// // #include "inja.hpp"
// // #include "json.hpp"

using namespace inja;
using json = nlohmann::json;

#define DISALLOW_COPY_AND_ASSIGN( TypeName )  \
          TypeName( const TypeName& ); \
          void operator=( const TypeName& )

/* Prototype */
int render_template();
int render( Environment& env, const std::string& tmpl_path, const nlohmann::json& vars );

/* -------------------------------------------------------------------------------- */
/* Internal module = anonymous namespace */
/* -------------------------------------------------------------------------------- */
namespace
{
// /** open with throwable */
// inline void open_with_throwable( const std::string& path, std::ifstream& file )
// {
//   file.exceptions( ( std::ifstream::failbit | std::ifstream::badbit ) );

//   try
//   {
//     file.open( path );
//   }
//   catch ( const std::ios_base::failure & /* e */ )
//   {
//     throw FileError( "Fail access to: '" + path + "'" );
//   }
// }

class Material
{
public:
  Material( const std::string& tmpls_path, const std::string& vars_file )
  : tmpls()
  , vars()
  {
    loadTemplateLists( tmpls_path );
    loadVariablesComponents( vars_file );
  }

  virtual ~Material()
  {
  }

  const std::vector< std::string > GetTemplateList() const
  {
    return tmpls;
  }

  const nlohmann::json GetVariablesComponents() const
  {
    return vars;
  }

private:
  /** Load templates */
  void loadTemplateLists( const std::string& tmpls_path )
  {
    std::cout << ">> Detecting templats...: " << tmpls_path << std::endl;
    for( const std::filesystem::directory_entry& i
          : std::filesystem::directory_iterator( tmpls_path ) )
    {
      // const std::string& file_name = i.path().filename().string();
      const std::string& apath_file = i.path().string();
      std::cout << " `- " << apath_file << std::endl;
      tmpls.push_back( apath_file );
    }
  }

  /** Load variables */
  void loadVariablesComponents( const std::string& vars_file )
  {
    std::cout << ">> Loading variables...: " << vars_file << std::endl;

    std::ifstream file;
    open_file_or_throw( vars_file, file );
    file >> vars;
    // nlohmann::json json_data;
    // file >> json_data;
    // std::cout << "json=" << json_data << std::endl;
    // vars.push_back( json_data );
  }

private:
  std::vector< std::string >  tmpls;
  nlohmann::json              vars;

private:
  Material();
  DISALLOW_COPY_AND_ASSIGN( Material );
};  /* class */
}   /* anonymous namespace */
/* -------------------------------------------------------------------------------- */

/**
 * Main
 *
 * All files in the following directories are rendered as templates.
 *    ./templates/
 *
 * Render the following files as a set of variables.
 *    ./variables/vars.json
 *
 * This variable set file is an array of root elements "vars",
 * and we can define the pattern to be rendered.
 *    { "vars" : [ { record1, ... }, { record2, ... }, ... ] }
 *
 * The rendering result will be outputted to the following directory.
 *    ./output/{api_name}/{template-file-name}
 *
 */
int main( int argc, char** argv )
{
  int ret = 0;

  try
  {
    std::cout << "# ----------------------------------------\n"
              << "Generete Your Code"
              << std::endl;
    ret = render_template();
    std::cout << "ret=" << ret << std::endl;
  }
  catch( const std::exception& e )
  {
    std::cerr << e.what() << '\n';
  }

  return ret;
}

/**
 *
 *
 *
 *
 */
int render_template()
{
  std::cout << ">> Begin process" << std::endl;

  Environment env { "./templates/", "./output/" };
  Material mat( "./templates", "./variables/vars.json" );

  // std::cout << "vars=" << mat.GetVariablesComponents()[ "vars" ].size() << std::endl;
  // std::cout << "vars=" << mat.GetVariablesComponents()[ "vars" ] << std::endl;

  for ( int i = 0; i < mat.GetVariablesComponents()[ "vars" ].size(); ++i )
  {
    for ( auto&& apath_tmpl_file : mat.GetTemplateList() )
    {
      render( env, apath_tmpl_file, mat.GetVariablesComponents()[ "vars" ][ i ] );
    }
  }

  return 0;
}

/**
 *
 *
 *
 */
int render( Environment& env, const std::string& apath_tmpl_file, const nlohmann::json& vars )
{
  int ret = 0;

  std::cout << ">> Begin rendering: " << apath_tmpl_file << std::endl;
  std::cout << "vars=" << vars << std::endl;

  /* Relative or absolute path depends on the first specification / Now relative */
  std::filesystem::path path_tmpl( apath_tmpl_file );
  // std::filesystem::path path_working( "." );

  /* abstract template file name (only file name) */
  std::string fname_tmpl = path_tmpl.filename().string();

  /* Create output file path */
  // std::string rpath_output = path_tmpl.parent_path().string();
  std::string specific_name = vars.at( "api_name" );
  // std::string fname_output = fname_tmpl;
  // std::string rpath_output_file = std::filesystem::absolute( path_working ).string()
  //                               + "/"
  //                               + specific_name
  //                               + "/"
  //                               + fname_output;

  std::string fname_output = specific_name + "@" + fname_tmpl;

  // rpath_output_file = R"(D:\workspace\repos\oss\proto-inja\output\)" + dname_specific
  //                   + R"(\)" + fname_output;

  try
  {
    env.write( fname_tmpl, vars, fname_output );
    std::cout << ">> Rendered: " << fname_tmpl << " -> " << fname_output << std::endl;

    // std::string rendered_output = env.render_file( fname_tmpl, vars );
    // std::ofstream output( rpath_output_file );
    // output << rendered_output;
    // output.write( rendered_output.data(), rendered_output.size() );

    // env.write( fname_tmpl, vars, rpath_output_file );
    // std::cout << ">> Rendered: " << fname_tmpl << " -> " << rpath_output_file << std::endl;

    // std::string rendered_output = env.render_file( fname_tmpl, vars );
    // std::cout << "result=\n"
    //           << "----- ----- ----- -----\n"
    //           << rendered_output
    //           << "\n----- ----- ----- -----"
    //           << std::endl;
    // if ( 0 == rendered_output.size() )
    // {
    //   std::cerr << "No rendering: " << apath_tmpl_file << '\n';
    // }
  }
  catch ( InjaError& e )
  {
    std::cout << "Exception: "  << e.what()
              // << " type="       << e.type
              // << " location="   << e.location.line << ":" << e.location.column
              // << " msg="        << e.message
              << std::endl;
  }



  // // // Render a string with json data
  // // std::string result = env.render("Hello {{ name }}!", data); // "Hello world!"

  // // Or directly read a template file
  // Template temp = env.parse_template("./templates/greeting.txt");
  // std::string result = env.render(temp, data); // "Hello world!"

  // data["name"] = "Inja";
  // std::string result = env.render(temp, data); // "Hello Inja!"

  // // Or read the template file (and/or the json file) directly from the environment
  // result = env.render_file("./templates/greeting.txt", data);
  // result = env.render_file_with_json_file("./templates/greeting.txt", "./data.json");

  // // Or write a rendered template file
  // env.write(temp, data, "./result.txt");
  // env.write_with_json_file("./templates/greeting.txt", "./data.json", "./result.txt");

  return ret;
}

