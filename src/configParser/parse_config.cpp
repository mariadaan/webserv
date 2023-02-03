#include "../configParser.hpp"

std::vector<Config>	parse_config(char const *conf_file)
{
	std::vector<std::string> file_vector;
	file_vector = get_file_vector(conf_file);
	// printStringVector(file_vector);

	if (!check_brackets(file_vector)) {
		std::cerr << "Found non matching bracket(s) in '" << conf_file << "' file" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::vector<std::vector<std::string> >	server_vector;
	server_vector = create_server_vector(file_vector);
	// printServerVector(server_vector);

	std::vector<Config>	config_vector;
	config_vector = set_config_vector(server_vector);	
	// for (size_t i = 0; i < config_vector.size(); i++)
	// {
	// 	config_vector[i].print_config_class();
	// 	std::cout << "-------------------------------------------" << std::endl;
	// }
	return (config_vector);
}

