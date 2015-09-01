/**
 * @file main.cpp
 * @author Miroslav Cibulka
 * @brief This is easy disassembler using BFP library
 */

#include <iostream>
#include <bfp.hpp>
#include <boost/program_options.hpp>
#include <iomanip>


using namespace std;
namespace po = ::boost::program_options;

int main(
    int argc,
    const char *argv[])
  {
    ::std::string __file;
    ::std::string __target;
    auto parser = ::bfp::Parser::get_unique_instance();

    try
      {
        po::options_description desc("Allowed options");
        desc.add_options()("help,h", "prints help")("list-targets,l",
                                                    "prints list of appropriate targets")(
            "list-symbols,s", "prints list of symbols")("file,f",
                                                        ::boost::program_options::value<::std::string>(),
                                                        "binary file")(
            "target,t", ::boost::program_options::value<::std::string>(),
            "target of binary file (default is first appropriate found)");
        po::positional_options_description p;
        p.add("file", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc)
                                                     .positional(p)
                                                     .run(), vm);
        po::notify(vm);

        if (vm.count("help"))
          {
            cout << desc << "\n";
            return EXIT_SUCCESS;
          }

        if (vm.count("file"))
          __file = vm["file"].as<::std::string>();
        else
          throw po::required_option("file");

        if (vm.count("list-targets"))
          for (auto &_tar : parser->getTargets(__file))
            printf("%s\n", _tar.c_str());

        if (vm.count("target"))
          __target = vm["target"].as<::std::string>();
        else
          __target = parser->getTargets(__file)[0];


        if (vm.count("list-symbols"))
          {
            auto file = parser->Open(__file, __target);
            if (file == nullptr)
              {
                ::std::cerr << "Bad file target" << ::std::endl;
                return EXIT_FAILURE;
              }
            ::std::ios::sync_with_stdio(false);
            ::std::cout << ::std::hex << ::std::setfill(' ');
            for (auto &sec : *file)
              {
                ::std::cout << ::std::endl << sec.getName() << "<0x" <<
                sec.getAddress() << ">:" << ::std::endl;
                for (auto &sym : sec)
                  {
                    ::std::cout << ::std::endl << "\t" << sym.getName() << " <0x" <<
                    sym.getValue() << ">" << ::std::endl;
                    if (sec.hasCodeOnly())
                      for (auto &_instr : sym)
                        {
                          ::std::cout << "\t\t" << "0x" << _instr.getAddress() <<
                          " " << ::std::setw(40) << _instr.getBinary() <<
                          _instr.getSignature() << ::std::endl;
                        }
                  }
              }
          }
      }
    catch (std::exception &e)
      {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
      }
    return EXIT_SUCCESS;
  }
