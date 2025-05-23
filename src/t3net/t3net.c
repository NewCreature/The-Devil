#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include <windows.h>
#endif
#include "t3net.h"
#include "internal.h"

static void t3net_destroy_data_entry_field(T3NET_DATA_ENTRY_FIELD * field);
static void t3net_destroy_data_entry(T3NET_DATA_ENTRY * entry);
static char _t3net_curl_command[1024] = {0};

char t3net_server_message[1024] = {0};

static char t3net_temp_dir[1024] = {0};

static FILE * _t3net_log_file = NULL;

static char * _t3net_default_url_runner(const char * url);

static char * (*_t3net_url_runner)(const char * url) = _t3net_default_url_runner;

static int run_system_command(char * command, const char * log_file)
{
	int ret;

	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};
		SECURITY_ATTRIBUTES sa;
		DWORD retvalue;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		HANDLE log_handle = CreateFile(log_file, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.hStdInput = NULL;
		si.hStdOutput = log_handle;
		si.hStdError = log_handle;
		si.wShowWindow = SW_HIDE;
		ret = CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &retvalue);
		if(log_handle)
		{
			CloseHandle(log_handle);
		}
		ret = retvalue;

	#else

		char final_command[1024];
		strcpy(final_command, command);
		if(log_file)
		{
			strcat(final_command, " > \"");
			strcat(final_command, log_file);
			strcat(final_command, "\"");
		}
		ret = system(final_command);

	#endif

	return ret;
}

void t3net_strcpy(char * dest, const char * src, int size)
{
	int c = 1;
	int pos = 0;

	while(c != '\0')
	{
		c = src[pos];
		dest[pos] = c;
		pos++;
		if(pos >= size)
		{
			pos = size - 1;
			break;
		}
	}
	dest[pos] = '\0';
}

char * t3net_load_file(const char * fn)
{
	char * data = NULL;
	FILE * fp = NULL;
	int size = 0;

	fp = fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fclose(fp);
	fp = NULL;

	data = malloc(size + 1);
	if(!data)
	{
		goto fail;
	}
	fp = fopen(fn, "rb");
	if(!fp)
	{
		goto fail;
	}
	fread(data, 1, size, fp);
	fclose(fp);
	data[size] = 0;
	remove(fn);
	return data;

	fail:
	{
		if(fp)
		{
			fclose(fp);
		}
		if(data)
		{
			free(data);
		}
		return NULL;
	}
}

int t3net_open_log_file(const char * fn)
{
	_t3net_log_file = fopen(fn, "wb");
	if(_t3net_log_file)
	{
		return 1;
	}
	return 0;
}

void t3met_close_log_file(void)
{
	if(_t3net_log_file)
	{
		fclose(_t3net_log_file);
	}
}

int t3net_setup(char * (*url_runner)(const char * url), const char * temp_dir)
{
	if(url_runner)
	{
		_t3net_url_runner = url_runner;
	}
	else
	{
		_t3net_url_runner = _t3net_default_url_runner;
	}
	if(temp_dir)
	{
		if(strlen(temp_dir) < 1024)
		{
			strcpy(t3net_temp_dir, temp_dir);
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

const char * t3net_get_curl_command(void)
{
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
		strcpy(_t3net_curl_command, "./curl.exe");
	#elif __APPLE__
		strcpy(_t3net_curl_command, "/usr/bin/curl");
	#else
		strcpy(_t3net_curl_command, "LD_LIBRARY_PATH=\"/lib\" curl");
	#endif
	return _t3net_curl_command;
}

T3NET_ARGUMENTS * t3net_create_arguments(void)
{
	T3NET_ARGUMENTS * arguments;

	arguments = malloc(sizeof(T3NET_ARGUMENTS));
	if(arguments)
	{
		memset(arguments, 0, sizeof(T3NET_ARGUMENTS));
	}
	return arguments;
}

void t3net_destroy_arguments(T3NET_ARGUMENTS * arguments)
{
	int i;

	for(i = 0; i < arguments->count; i++)
	{
		free(arguments->key[i]);
		free(arguments->val[i]);
	}
	free(arguments);
}

static const char * url_encode_char(int c, char * buf, int size)
{
	if(
		(c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		(c >= '0' && c <= '9') ||
		(c == '-') ||
		(c == '.') ||
		(c == '_') ||
		(c == '~')
	)
	{
		sprintf(buf, "%c", c);
	}
	else
	{
		sprintf(buf, "%%%2X", c);
	}
	return buf;
}

static int escape_strlen(const char * s)
{
	char buf[16] = {0};
	int i;
	int l = 0;

	for(i = 0; i < strlen(s); i++)
	{
		url_encode_char(s[i], buf, 16);
		l += strlen(buf);
	}
	return l;
}

char * t3net_escape(const char * s)
{
	char buf[16] = {0};
	int i;
	char * escape_s = NULL;
	const char * fragment = NULL;
	int l;

	l = escape_strlen(s);
	if(!l)
	{
		goto fail;
	}
	escape_s = malloc(l + 1);
	if(!escape_s)
	{
		goto fail;
	}
	escape_s[0] = 0;
	for(i = 0; i < strlen(s); i++)
	{
		fragment = url_encode_char(s[i], buf, 16);
		strcat(escape_s, fragment);
	}
	return escape_s;

	fail:
	{
		if(escape_s)
		{
			free(escape_s);
		}
		return NULL;
	}
}

int t3net_add_argument(T3NET_ARGUMENTS * arguments, const char * key, const char * val)
{
	if(arguments->count < T3NET_MAX_ARGUMENTS)
	{
		arguments->key[arguments->count] = t3net_escape(key);
		if(!arguments->key[arguments->count])
		{
			return 0;
		}
		arguments->val[arguments->count] = t3net_escape(val);
		if(!arguments->val[arguments->count])
		{
			free(arguments->key[arguments->count]);
			return 0;
		}
		arguments->count++;
		return 1;
	}
	return 0;
}

static int t3net_get_line_length(const char * data, unsigned int text_pos)
{
	int length = 0;
	int c, l;

	l = strlen(data);

	while(text_pos < l)
	{
		c = data[text_pos];
		if(c != '\r')
		{
			length++;
		}
		else if(c == '\0')
		{
			return length;
		}
		else
		{
			text_pos++;
			c = data[text_pos];
			if(c != '\n')
			{
				return -1;
			}
			else
			{
				return length;
			}
		}
		text_pos++;
	}
	return -1;
}

/* Read a line of text and put it in a separate buffer. Lines are expected to end in "\r\n". */
int t3net_read_line(const char * data, char * output, int data_max, int output_max, unsigned int * text_pos)
{
	int outpos = 0;
	int c;

	while(1)
	{
		c = data[*text_pos];
		if(c != '\r')
		{
			output[outpos] = c;
		}
		else
		{
			output[outpos] = '\0';
			(*text_pos)++;
			c = data[*text_pos];
			if(c == '\n')
			{
				(*text_pos)++;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		outpos++;
		if(outpos >= output_max)
		{
			outpos--;
			output[outpos] = '\0';
			return 1;
		}
		(*text_pos)++;
		if(*text_pos >= data_max)
		{
			return 0;
		}
	}
	return 0;
}

char * t3net_get_line(const char * data, int data_max, unsigned int * text_pos)
{
	char * text_line = NULL;
	int bytes = 0;

	bytes = t3net_get_line_length(data, *text_pos) + 1;
	if(bytes > 0)
	{
		text_line = malloc(bytes);
	}
	if(text_line)
	{
		t3net_read_line(data, text_line, data_max, bytes, text_pos);
	}
	return text_line;
}

static void discard_temp_element(T3NET_TEMP_ELEMENT * element)
{
	if(element->name)
	{
		free(element->name);
		element->name = NULL;
	}
	if(element->data)
	{
		free(element->data);
		element->data = NULL;
	}
}

static int get_temp_element(const char * data, T3NET_TEMP_ELEMENT * element, int data_max)
{
	int outpos = 0;
	int c;
	int read_pos = 1; // skip first byte

	element->name = malloc(data_max);
	if(!element->name)
	{
		goto fail;
	}
	element->data = malloc(data_max);
	if(!element->data)
	{
		goto fail;
	}
	memset(element->name, 0, data_max);
	memset(element->data, 0, data_max);

	/* read element name */
	while(read_pos < data_max)
	{
		c = data[read_pos];

		if(c == ':')
		{
			read_pos += 2;
			break;
		}
		else
		{
			element->name[outpos] = c;
			outpos++;
			read_pos++;
		}
	}
	if(read_pos >= data_max)
	{
		goto fail;
	}

	/* read element data */
	outpos = 0;
	while(c != '\0' && read_pos < data_max)
	{
		c = data[read_pos];

		element->data[outpos] = c;
		outpos++;
		read_pos++;
	}
	return 1;

	fail:
	{
		discard_temp_element(element);
		return 0;
	}
}

static int get_arguments_length(const T3NET_ARGUMENTS * arguments)
{
	int i;
	int size = 0;

	if(arguments)
	{
		for(i = 0; i < arguments->count; i++)
		{
			size += 1;
			size += strlen(arguments->key[i]);
			size += 1;
			size += strlen(arguments->val[i]);
		}
	}
	return size;
}

static char * _t3net_default_url_runner(const char * url)
{
	char * curl_command = malloc(strlen(url) + 1024);
	char temp_path[1024] = {0};

	if(curl_command)
	{
		sprintf(temp_path, "%st3net.out", t3net_temp_dir);
		sprintf(curl_command, "%s --connect-timeout %d \"%s\" --silent --output \"%s\"", t3net_get_curl_command(), T3NET_TIMEOUT_TIME, url, temp_path);
		run_system_command(curl_command, NULL);
		free(curl_command);
		return t3net_load_file(temp_path);
	}
	return NULL;
}

char * t3net_get_raw_data(const char * url, const T3NET_ARGUMENTS * arguments)
{
	char * final_url = NULL;
	char * ret;
	int final_url_size;
	int i;

	final_url_size = strlen(url) + get_arguments_length(arguments) + 1;
	final_url = malloc(final_url_size);
	if(final_url)
	{
		strcpy(final_url, url);
		if(arguments)
		{
			for(i = 0; i < arguments->count; i++)
			{
				if(i == 0)
				{
					strcat(final_url, "?");
				}
				else
				{
					strcat(final_url, "&");
				}
				strcat(final_url, arguments->key[i]);
				strcat(final_url, "=");
				strcat(final_url, arguments->val[i]);
			}
		}
		ret = _t3net_url_runner(final_url);
		if(_t3net_log_file)
		{
			if(ret)
			{
				fwrite(ret, 1, strlen(ret), _t3net_log_file);
				fputc('\n', _t3net_log_file);
			}
			else
			{
				fwrite("no text\n", 1, strlen("no text\n"), _t3net_log_file);
			}
			fflush(_t3net_log_file);
		}
		free(final_url);
		return ret;
	}

	return NULL;
}

static int t3net_count_data_entries_in_string(const char * s, int * field_max)
{
	int c = -1;
	int pos = 0;
	int step = 0;
	int entries = -1;
	int fields = 0;

	*field_max = 0;
	while(c != '\0')
	{
		c = s[pos];
		if(step == 0)
		{
			if(c == '\r')
			{
				step++;
			}
		}
		else if(step == 1)
		{
			if(c == '\n')
			{
				step++;
			}
			else
			{
				step = 0;
			}
		}
		else if(step == 2)
		{
			if(c == '\r')
			{
				step++;
			}
			else
			{
				fields++;
				step = 0;
			}
		}
		else if(step == 3)
		{
			if(c == '\n')
			{
				entries++;
				if(fields > *field_max)
				{
					*field_max = fields;
				}
				fields = 1;
			}
			step = 0;
		}
		pos++;
	}
	return entries;
}

static T3NET_DATA_ENTRY * t3net_create_data_entry(int max_fields)
{
	T3NET_DATA_ENTRY * entry;
	int i;

	entry = malloc(sizeof(T3NET_DATA_ENTRY));
	if(!entry)
	{
		goto error_out;
	}
	entry->field = malloc(sizeof(T3NET_DATA_ENTRY_FIELD *) * max_fields);
	if(!entry->field)
	{
		goto error_out;
	}
	memset(entry->field, 0, sizeof(T3NET_DATA_ENTRY_FIELD *) * max_fields);
	for(i = 0; i < max_fields; i++)
	{
		entry->field[i] = malloc(sizeof(T3NET_DATA_ENTRY_FIELD));
		if(!entry->field[i])
		{
			goto error_out;
		}
		memset(entry->field[i], 0, sizeof(T3NET_DATA_ENTRY_FIELD));
		entry->fields = max_fields;
	}
	return entry;

	error_out:
	{
		t3net_destroy_data_entry(entry);
		return NULL;
	}
}

static T3NET_DATA * t3net_create_data(int max_entries, int max_fields)
{
	T3NET_DATA * data;
	int i;

	data = malloc(sizeof(T3NET_DATA));
	if(!data)
	{
		return NULL;
	}
	memset(data, 0, sizeof(T3NET_DATA));

	/* allocate space for data entries */
	data->entry = NULL;
	if(max_entries > 0)
	{
		data->entry = malloc(sizeof(T3NET_DATA_ENTRY *) * max_entries);
		if(!data->entry)
		{
			goto error_out;
		}
		for(i = 0; i < max_entries; i++)
		{
			data->entry[i] = t3net_create_data_entry(max_fields);
			if(!data->entry[i])
			{
				goto error_out;
			}
		}
	}
	data->entries = max_entries;
	return data;

	error_out:
	{
		t3net_destroy_data(data);
		return NULL;
	}
}

T3NET_DATA * t3net_get_data_from_string(const char * raw_data)
{
	int text_max;
	T3NET_DATA * data = NULL;
	char * current_line;
	int entries = 0;
	int fields = 0;
	int l, size;
	int field = 0;
	unsigned int text_pos = 0;
	int ecount = -1;
	T3NET_TEMP_ELEMENT element;
	int empty_data = 0;

	if(!raw_data)
	{
		return NULL;
	}

	/* check for error */
	if(strlen(raw_data) >= 5 && !strncmp(raw_data, "Error", 5))
	{
		empty_data = 1;
	}
	else if(strlen(raw_data) >= 3 && !strncmp(raw_data, "ack", 3))
	{
		empty_data = 1;
	}
	if(empty_data)
	{
		data = t3net_create_data(0, fields);
		if(data)
		{
			data->header = malloc(strlen(raw_data) + 1);
			if(data->header)
			{
				strcpy(data->header, raw_data);
			}
		}
		return data;
	}

	entries = t3net_count_data_entries_in_string(raw_data, &fields);
	if(entries < 0)
	{
		goto error_out;
	}
	data = t3net_create_data(entries, fields);
	if(!data)
	{
		return NULL;
	}

	text_pos = 0;
	text_max = strlen(raw_data) + 1;

	/* read header */
	current_line = t3net_get_line(raw_data, text_max, &text_pos);
	if(!current_line)
	{
		goto error_out;
	}
	data->header = current_line;

	while(1)
	{
		current_line = t3net_get_line(raw_data, text_max, &text_pos);
		if(current_line)
		{
			/* empty line signifies new entry */
			l = strlen(current_line);
			if(l <= 0)
			{
				ecount++;
				field = 0;
			}

			/* get fields of the current element */
			else if(ecount >= 0 && ecount < data->entries && field < data->entry[ecount]->fields)
			{
				size = l + 1;
				if(get_temp_element(current_line, &element, size))
				{
					/* copy field name */
					size = strlen(element.name);
					data->entry[ecount]->field[field]->name = strdup(element.name);

					/* copy field data */
					size = strlen(element.data);
					data->entry[ecount]->field[field]->data = strdup(element.data);

					discard_temp_element(&element);
					field++;
				}
			}
		}
		else
		{
			break;
		}

		/* get out if we've reached the end of the data */
		if(text_pos >= text_max)
		{
			break;
		}
	}
	return data;

	error_out:
	{
		t3net_destroy_data(data);
		return NULL;
	}
}

T3NET_DATA * t3net_get_data(const char * url, const T3NET_ARGUMENTS * arguments)
{
	char * raw_data;
	T3NET_DATA * data = NULL;

	raw_data = t3net_get_raw_data(url, arguments);
	if(raw_data)
	{
		data = t3net_get_data_from_string(raw_data);
		free(raw_data);
	}
	return data;
}

static void t3net_destroy_data_entry_field(T3NET_DATA_ENTRY_FIELD * field)
{
	if(field)
	{
		if(field->name)
		{
			free(field->name);
		}
		if(field->data)
		{
			free(field->data);
		}
		free(field);
	}
}

static void t3net_destroy_data_entry(T3NET_DATA_ENTRY * entry)
{
	int j;

	if(entry)
	{
		if(entry->field)
		{
			for(j = 0; j < entry->fields; j++)
			{
				t3net_destroy_data_entry_field(entry->field[j]);
			}
			free(entry->field);
		}
		free(entry);
	}
}

void t3net_destroy_data(T3NET_DATA * data)
{
	int i;

	if(data)
	{
		if(data->entry)
		{
			for(i = 0; i < data->entries; i++)
			{
				t3net_destroy_data_entry(data->entry[i]);
			}
			free(data->entry);
		}
		if(data->header)
		{
			free(data->header);
		}
		free(data);
	}
}

const char * t3net_get_error(T3NET_DATA * data)
{
	if(data->header && strlen(data->header) > 7 && !strncmp(data->header, "Error", 5))
	{
		return &data->header[7];
	}
	return NULL;
}

const char * t3net_get_data_entry_field(T3NET_DATA * data, int entry, const char * field_name)
{
	int i;

	if(entry < data->entries)
	{
		for(i = 0; i < data->entry[entry]->fields; i++)
		{
			if(data->entry[entry]->field[i]->name && !strcmp(data->entry[entry]->field[i]->name, field_name))
			{
				return data->entry[entry]->field[i]->data;
			}
		}
	}
	return NULL;
}
