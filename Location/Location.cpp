#include "Location.hpp"

Location::Location()
{
	this->path = "";
	this->root = "";
	this->index = "";
	this->autoindex = false;
	this->returnPath = "";
	this->alias = "";
	this->client_max_body_size = MAX_REQUEST_LENGTH;
	this->methods.reserve(5);
	this->methods.push_back(1);
	this->methods.push_back(0);
	this->methods.push_back(0);
	this->methods.push_back(0);
	this->methods.push_back(0);
}

Location::~Location()
{
}

