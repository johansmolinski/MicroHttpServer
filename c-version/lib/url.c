/* This file defines the server application functions (SAFs). */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "url.h"

// Helper macro for url parsing
#define SPLIT(right, delm) {\
	strsep(&token, delm);\
	if (token == NULL) right = empty;\
	else right = token; \
	token = url_start; \
}

const char *supported_apiversions[] = {
    "v1",
    //"v2",
    NULL
};

struct Parameter *parse_querystring(char *querystring_copy, size_t *parameters_len) {
	struct Parameter *parameters;

	char *token, *name, *value;
	static const char *empty = "";

	*parameters_len = 0;
	if (strlen(querystring_copy) == 0) {
		return NULL;
	}

	*parameters_len = 1;
	for (int i = 0; i < strlen(querystring_copy); i++) {
		if (querystring_copy[i] == '&') (*parameters_len)++;
	}

	parameters = (struct Parameter *)malloc(*parameters_len * sizeof(struct Parameter));

	int index = 0;
	while ((value = token = strsep(&querystring_copy, "&")) != NULL) {
		name = strsep(&value, "=");
		parameters[index].name = name;
		if (value == NULL) {
			parameters[index].value = empty;
		} else {
			parameters[index].value = value;
		}
		index++;
	}

	return parameters;
}

struct UrlComponents *new_url_components(
		char *url_copy,
		const char *apiversion,
		const char *route,
		const char *path,
		const char *command,
		const char *querystring) {
	struct UrlComponents *c;

	if (!(c = (struct UrlComponents *)malloc(sizeof(*c)))) {
		return NULL;
	};
	c->method = HTTP_GET;
	c->apiversion = apiversion;
	c->route = route;
	c->path = path;
	c->command = command;
	c->querystring = querystring;
	c->url_copy = url_copy;
	c->querystring_copy = strdup(querystring);

	c->parameters = parse_querystring(c->querystring_copy, &(c->parameters_len));

	return c;
}

void delete_url_components(struct UrlComponents *components) {
	free(components->parameters);
	components->parameters = NULL;
	free(components->url_copy);
	components->url_copy = NULL;
	free(components->querystring_copy);
	components->querystring_copy = NULL;
	free(components);
	components = NULL; // only sets components to NULL on the stack, and then leaves, abandoning the stack
}

struct UrlComponents *parse_url_header(HTTPReqHeader *hdr)
{
	struct UrlComponents *c = parse_url(hdr->URI);
	if (c) {
		c->method = hdr->Method;
	}
	return c;
}

struct UrlComponents *parse_url(const char *url) {
	struct UrlComponents *components;
	const char *apiversion, *route, *path, *command, *querystring;

	int supported_version = 0;

	char *url_copy, *url_start, *token;

	if (url[0] == '/') url++;

	url_copy = token = url_start = strdup(url);
	static const char *empty = "";

	// Consume apiversion first (a little dirty, but we need to handle it some way)
	apiversion = strsep(&token, "/");
	url_start = token = url_start+strlen(apiversion)+1;

	for (int i = 0; supported_apiversions[i] != NULL; i++) {
		if (!strcmp(apiversion, supported_apiversions[i])) {
			supported_version = 1;
		}
	}

	if (!supported_version) {
		free(url_copy);
		return NULL;
	}

	// Separate querystring
	SPLIT(querystring, "?");

	// Separate command
	SPLIT(command, ":");

	// Separate path
	SPLIT(path, "/");

	// Route is all that is left
	route = url_start;

	components = new_url_components(url_copy, apiversion, route, path, command, querystring);

	return components;
}
