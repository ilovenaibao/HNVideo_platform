#include "HNProtocol.h"
#include "./leak_detector/leak_detector_c.h"

//////////////////////////////////////////////////////////////////////////
HNProtocol::HNProtocol()
{

}

HNProtocol::~HNProtocol()
{

}

void HNProtocol::CreateXmlBuffer(_OUT XML_Char *p_out, _IN XmlElement *all_elements)
{
	scew_tree		*xml_tree			= NULL;
	scew_element	*xml_root			= NULL;
	scew_element	*xml_element		= NULL;
	
	/**
	* Create an empty XML tree in memory, and add a root element
	* "scew_test".
	*/
	xml_tree = scew_tree_create ();
#ifdef XML_UNICODE_WCHAR_T
	scew_tree_set_xml_encoding(tree, _XT("UTF-16"));
#endif /* XML_UNICODE_WCHAR_T */
	if (NULL == xml_tree) {
		return;
	}
	int element_count = 0;
	for (element_count =0; XML_EOF != all_elements[element_count].element_deep; element_count++)
	{
		all_elements[element_count].cur_parent_key_index = 0;
		switch (all_elements[element_count].element_deep) {
		case XML_ROOT:
			if (NULL != all_elements[element_count].element_key && 0 == element_count) {
				xml_root = scew_tree_set_root (xml_tree, _XT(all_elements[element_count].element_key));
			} else {
				printf("error: not found xml root\n");
				return;
			}
			break;
		case XML_ELEMENT:
		case XML_SUB_ELEMENT:
			AddFrameInfoToXml(xml_tree, &all_elements[element_count]);
			break;
		case XML_EOF:
			break;
		}
	}
	
	SaveXmlIntoBuffer(p_out, xml_tree);
	// free other memory
	/* Remember to free tree (scew_parser_free does not free it). */
	if (NULL != xml_tree) {
		scew_tree_free (xml_tree);
		xml_tree = NULL;
	}
}

XML_Char* HNProtocol::CreateXmlBuffer(_IN XmlElement *all_elements)
{
	scew_tree		*xml_tree			= NULL;
	scew_element	*xml_root			= NULL;
	scew_element	*xml_element		= NULL;
	
	/**
	* Create an empty XML tree in memory, and add a root element
	* "scew_test".
	*/
	xml_tree = scew_tree_create ();
#ifdef XML_UNICODE_WCHAR_T
	scew_tree_set_xml_encoding(tree, _XT("UTF-16"));
#endif /* XML_UNICODE_WCHAR_T */
	if (NULL == xml_tree) {
		return NULL;
	}
	int element_count = 0;
	for (element_count =0; XML_EOF != all_elements[element_count].element_deep; element_count++)
	{
		all_elements[element_count].cur_parent_key_index = 0;
		switch (all_elements[element_count].element_deep) {
		case XML_ROOT:
			if (NULL != all_elements[element_count].element_key && 0 == element_count) {
				xml_root = scew_tree_set_root (xml_tree, _XT(all_elements[element_count].element_key));
			} else {
				printf("error: not found xml root\n");
				return NULL;
			}
			break;
		case XML_ELEMENT:
		case XML_SUB_ELEMENT:
			AddFrameInfoToXml(xml_tree, &all_elements[element_count]);
			break;
		case XML_EOF:
			break;
		}
	}
	
	XML_Char *ret = SaveXmlIntoBuffer(xml_tree);
	// free other memory
	/* Remember to free tree (scew_parser_free does not free it). */
	if (NULL != xml_tree) {
		scew_tree_free (xml_tree);
		xml_tree = NULL;
	}

	return ret;
}

int HNProtocol::AddFrameInfoToXml(_IN scew_tree *xml_tree, _IN XmlElement *pelement)
{
	int ret = 0;
	scew_attribute *attribute = NULL;

	if (NULL == pelement) {
		return ret;
	}
	/* Add an element and set element contents. */
	AddXmlElement_(scew_tree_root(xml_tree), pelement, ALL_OF_KIND);

	ret = 1;
	return ret;
}

void HNProtocol::SaveXmlIntoBuffer(_OUT XML_Char *xml_buffer, _IN scew_tree *m_xml_tree, _IN unsigned int buf_size)
{
	scew_printer	*xml_printer		= NULL;
	scew_writer		*xml_writer			= NULL;

	/* Save the XML tree to a buffer and print it to standard output. */
	xml_writer = scew_writer_buffer_create (xml_buffer, buf_size);
	if (NULL == xml_writer)
	{
		printf ("Unable to create writer buffer\n");
		return;
	}
	xml_printer = scew_printer_create (xml_writer);
	if (NULL == xml_printer)
	{
		printf ("Unable to create printer\n");
		return;
	}
	/* We should check for errors here. */
	(void) scew_printer_print_tree(xml_printer, m_xml_tree);
	if (NULL != xml_writer) {
		scew_writer_free (xml_writer);
	}
	if (NULL != xml_printer) {
		scew_printer_free (xml_printer);
	}
}

XML_Char* HNProtocol::SaveXmlIntoBuffer(_IN scew_tree *m_xml_tree, _IN int buf_size)
{
	scew_printer	*xml_printer		= NULL;
	scew_writer		*xml_writer			= NULL;

	/* Save the XML tree to a buffer and print it to standard output. */
	XML_Char *xml_buffer = (XML_Char *) malloc (buf_size);
	xml_writer = scew_writer_buffer_create (xml_buffer, buf_size);
	if (NULL == xml_writer)
	{
		if (NULL != xml_buffer) {
			free(xml_buffer);
			xml_buffer = NULL;
		}
		printf ("Unable to create writer buffer\n");
		return xml_buffer;
	}
	xml_printer = scew_printer_create (xml_writer);
	if (NULL == xml_printer)
	{
		if (NULL != xml_buffer) {
			free(xml_buffer);
			xml_buffer = NULL;
		}
		printf ("Unable to create printer\n");
		return xml_buffer;
	}
	/* We should check for errors here. */
	(void) scew_printer_print_tree(xml_printer, m_xml_tree);
	if (NULL != xml_writer) {
		scew_writer_free (xml_writer);
	}
	if (NULL != xml_printer) {
		scew_printer_free (xml_printer);
	}

	return xml_buffer;
}

XML_Char* HNProtocol::save_xml_into_buffer(_IN scew_tree *xml_tree, _IN int buf_size)
{
	scew_writer *xml_writer = NULL;
	scew_printer *xml_printer = NULL;

	/* Save the XML tree to a buffer and print it to standard output. */
	XML_Char *xml_buffer = (XML_Char *) malloc (buf_size);
	xml_writer = scew_writer_buffer_create (xml_buffer, buf_size);
	if (NULL == xml_writer)
	{
		if (NULL != xml_buffer) {
			free(xml_buffer);
			xml_buffer = NULL;
		}
		printf ("Unable to create writer buffer\n");
		return xml_buffer;
	}
	xml_printer = scew_printer_create (xml_writer);
	if (NULL == xml_printer)
	{
		if (NULL != xml_buffer) {
			free(xml_buffer);
			xml_buffer = NULL;
		}
		printf ("Unable to create printer\n");
		return xml_buffer;
	}
	/* We should check for errors here. */
	(void) scew_printer_print_tree(xml_printer, xml_tree);

	scew_writer_free (xml_writer);
	scew_printer_free (xml_printer);

	return xml_buffer;
}

void HNProtocol::PrintXmlBuffer(_IN XML_Char *xml_buffer)
{
	// print xml buffer
	if (NULL == xml_buffer) {
		scew_printf (_XT("%s"), "null");
	} else {
		char p_out[MAX_OUTPUT_BUFFER_] = { '\0' };
		UTF_8ToGB2312(p_out, xml_buffer, strlen(xml_buffer));

		scew_printf (_XT("%s"), p_out);
	}
}

int HNProtocol::AddXmlElement_(_IN scew_element *element, _IN XmlElement *add_element, _IN int add_kind)
{
	int ret = 0;
	XML_Char const *element_key = NULL;
	XML_Char const *contents = NULL;

	unsigned int indent = 0;

	scew_list *list = NULL;

	if (element == NULL || NULL == add_element)
	{
		return ret;
	}
	if (0 == strcmp(add_element->element_key, "") ||
		0 == strcmp(add_element->parent_key[0], "")) {
		return ret;
	}
	if (NULL == add_element->cur_parent_key) {
		add_element->cur_parent_key = *add_element->parent_key;
		add_element->cur_parent_key_index = 0;
	}

	// debug
	if (0 == strcmp("Device2", add_element->element_key)) {
		int kkk = 0;
	}
	// end

	/* Prints the starting element tag with its attributes. */
	element_key = scew_element_name(element);
	//print_attributes (element);
	contents = scew_element_contents(element);
	// compare key_name is right
	list = scew_element_children(element);
	scew_attribute *attribute = NULL;
	if (0 == strcmp(element_key, add_element->cur_parent_key)) {
		if (NULL == list) {
			AddXmlElement_2(element, add_element, add_kind);
			add_element->cur_parent_key_index = -1;
			ret = 2;
			return ret;
		}
		if (-1 == add_element->cur_parent_key_index) {
			ret = 1;
			return ret;
		}
		add_element->cur_parent_key_index++;
		if (0 <= add_element->cur_parent_key_index &&
			0 != strcmp(add_element->parent_key[add_element->cur_parent_key_index], "")) {
			add_element->cur_parent_key = add_element->parent_key[add_element->cur_parent_key_index];
		} else {
			if (0 <= add_element->cur_parent_key_index) {
				add_element->cur_parent_key = add_element->element_key;
				add_element->cur_parent_key_index = -1;
				AddXmlElement_2(element, add_element, add_kind);
				ret = 2;
				return ret;
			} else {
				ret = 1;
				return ret;
			}
		}
	}

	/**
	* Call print_element function again for each child of the current
	* element.
	*/
	while (list != NULL)
	{
		scew_element *child = (scew_element *)scew_list_data(list);
		ret = AddXmlElement_(child, add_element, add_kind);
		if (1 == ret) {
			// add element into buffer
			/* Add an element and set element contents. */
			if (-2 == add_element->cur_parent_key_index || NULL == child) {
				return ret;
			}
			AddXmlElement_2(child, add_element, add_kind);
			add_element->cur_parent_key_index = -2;
			break;
		} else if (2 == ret) {
			break;
		}
		indent++;
		list = scew_list_next (list);
		if (NULL == list && 0 == ret && (-1 == add_element->cur_parent_key_index || 0 == add_element->cur_parent_key_index)) {
			AddXmlElement_2(element, add_element, add_kind);
			add_element->cur_parent_key_index = -2;
			ret = 1;
			return ret;
		}
	}
	
	return ret;
}

int HNProtocol::AddXmlElement_2(_IN scew_element *element, _IN XmlElement *add_element, _IN int add_kind)
{
	int ret = 0;
	scew_attribute *attribute = NULL;

	switch(add_kind) {
	case SET_ELEMENT:
		if (NULL != add_element->key_attrib) {
			int key_attrib_count = 0;
			for (key_attrib_count = 0; ;key_attrib_count++) {
				if (NULL == add_element->key_attrib[key_attrib_count] ||
					0 == strcmp(add_element->key_attrib[key_attrib_count], "")) {
						break;
				}
				if (NULL == add_element->key_attrib_content[key_attrib_count]) {
					attribute = scew_attribute_create (_XT(add_element->key_attrib[key_attrib_count]), 
						_XT(""));
				} else {
					attribute = scew_attribute_create (_XT(add_element->key_attrib[key_attrib_count]), 
						_XT(add_element->key_attrib_content[key_attrib_count]));
				}
				
				scew_element_add_attribute (element, attribute);
			}
		}
		scew_element_set_contents (element, _XT(add_element->element_info));
		break;
	case ADD_ELEMENT:

		break;
	case ALL_OF_KIND:
		element = scew_element_add (element, _XT(add_element->element_key));
		if (NULL == element) {
			return ret;
		}
		if (NULL != add_element->key_attrib) {
			int key_attrib_count = 0;
			for (key_attrib_count = 0; ;key_attrib_count++) {
				if (NULL == add_element->key_attrib[key_attrib_count] ||
					0 == strcmp(add_element->key_attrib[key_attrib_count], "")) {
						break;
				}
				if (NULL == add_element->key_attrib_content[key_attrib_count]) {
					attribute = scew_attribute_create (_XT(add_element->key_attrib[key_attrib_count]), 
						_XT(""));
				} else {
					attribute = scew_attribute_create (_XT(add_element->key_attrib[key_attrib_count]), 
						_XT(add_element->key_attrib_content[key_attrib_count]));
				}
				scew_element_add_attribute (element, attribute);
			}
		}
		if (NULL == add_element->element_info) {
			scew_element_set_contents(element, _XT(""));
		} else {
			scew_element_set_contents(element, _XT(add_element->element_info));
		}
		add_element->cur_parent_key_index = -1;
		ret = 1;
		break;
	}

	return ret;
}

XML_Char* HNProtocol::AddElementInXmlBuffer(XML_Char *buffer_in, XmlElement *add_element, int add_kind)
{
	scew_reader *reader = NULL;
	scew_parser *parser = NULL;
	scew_tree *tree = NULL;

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
	/* Change stdout to Unicode before writing anything. */
	_setmode(_fileno(stdout), _O_U16TEXT);
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

	/* Creates an SCEW parser. This is the first function to call. */
	parser = scew_parser_create();
	if (NULL == parser) {
		printf("xml parser = NULL\n");
		return buffer_in;
	}
	scew_parser_ignore_whitespaces (parser, SCEW_TRUE);

	/* Loads an XML file. */
	reader = scew_reader_buffer_create(buffer_in, strlen(buffer_in));
	//reader = scew_reader_file_create("testXML.xml");
	if (reader == NULL)
	{
		scew_error code = scew_error_code ();
		scew_printf (_XT("Unable to load file (error #%d: %s)\n"),
			code, scew_error_string (code));
		return buffer_in;
	}
	tree = scew_parser_load (parser, reader);
	if (tree == NULL)
	{
		scew_error code = scew_error_code ();
		scew_printf (_XT("Unable to parse file (error #%d: %s)\n"),
			code, scew_error_string (code));
		if (code == scew_error_expat)
		{
			enum XML_Error expat_code = scew_error_expat_code (parser);
			scew_printf (_XT("Expat error #%d (line %d, column %d): %s\n"),
				expat_code,
				scew_error_expat_line (parser),
				scew_error_expat_column (parser),
				scew_error_expat_string (expat_code));
		}

		/* Frees the SCEW parser and reader. */
		if (NULL != reader) {
			scew_reader_free (reader);
		}
		if (NULL != parser) {
			scew_parser_free (parser);
		}
		
		return buffer_in;
	}
	AddXmlElement_(scew_tree_root(tree), add_element, add_kind);
	// save xml to buf
	XML_Char *p_xml_buffer = NULL;
	p_xml_buffer = save_xml_into_buffer(tree, strlen(buffer_in));
	if (NULL != buffer_in) {
		free(buffer_in);
		buffer_in = NULL;
	}
	buffer_in = p_xml_buffer;
	p_xml_buffer = NULL;

	/* Remember to free tree (scew_parser_free does not free it). */
	scew_tree_free (tree);

	/* Frees the SCEW parser and reader. */
	scew_reader_free (reader);
	scew_parser_free (parser);

	return buffer_in;
}

int HNProtocol::GetElementInXmlBuffer(_OUT XmlElement *in_element, _IN XML_Char *buffer_in)
{
	int ret = 0;
	scew_reader *reader = NULL;
	scew_parser *parser = NULL;
	scew_tree *tree = NULL;

	if (NULL == buffer_in || 0 >= strlen(buffer_in)) {
		return ret;
	}

#if defined(_MSC_VER) && defined(XML_UNICODE_WCHAR_T)
	/* Change stdout to Unicode before writing anything. */
	_setmode(_fileno(stdout), _O_U16TEXT);
#endif /* _MSC_VER && XML_UNICODE_WCHAR_T */

	/* Creates an SCEW parser. This is the first function to call. */
	parser = scew_parser_create();
	if (NULL == parser) {
		printf("xml parser = NULL\n");
		return ret;
	}
	scew_parser_ignore_whitespaces (parser, SCEW_TRUE);

	/* Loads an XML file. */
	reader = scew_reader_buffer_create(buffer_in, strlen(buffer_in));
	if (reader == NULL)
	{
		scew_error code = scew_error_code ();
		scew_printf (_XT("Unable to load file (error #%d: %s)\n"),
			code, scew_error_string (code));
		return ret;
	}
	tree = scew_parser_load (parser, reader);
	if (tree == NULL)
	{
		scew_error code = scew_error_code ();
		scew_printf (_XT("Unable to parse file (error #%d: %s)\n"),
			code, scew_error_string (code));
		if (code == scew_error_expat)
		{
			enum XML_Error expat_code = scew_error_expat_code (parser);
			scew_printf (_XT("Expat error #%d (line %d, column %d): %s\n"),
				expat_code,
				scew_error_expat_line (parser),
				scew_error_expat_column (parser),
				scew_error_expat_string (expat_code));
		}

		/* Frees the SCEW parser and reader. */
		if (NULL != reader) {
			scew_reader_free (reader);
			reader = NULL;
		}
		if (NULL != parser) {
			scew_parser_free (parser);
			parser = NULL;
		}
		return ret;
	}
	for (int i = 0; XML_EOF != in_element[i].element_deep; i++) {
		if (XML_ELEMENT != in_element[i].element_deep && XML_ROOT != in_element[i].element_deep) {
			break;
		}
		in_element[i].cur_parent_key = in_element[i].parent_key[0];
		in_element[i].cur_parent_key_index = 0;
		int get_flag = GetXmlElement_(scew_tree_root(tree), &in_element[i]);
		if (0 == get_flag) {
			if (XML_ELEMENT == in_element[i].element_deep) {
				in_element[i].element_deep = XML_EOF;
				ret = 0;
				break;
			}
		}
		ret = 1;
	}
	
	/* Remember to free tree (scew_parser_free does not free it). */
	if (NULL != tree) {
		scew_tree_free (tree);
		tree = NULL;
	}
	/* Frees the SCEW parser and reader. */
	if (NULL != reader) {
		scew_reader_free (reader);
		reader = NULL;
	}
	if (NULL != parser) {
		scew_parser_free (parser);
		parser = NULL;
	}

	return ret;
}

int HNProtocol::GetXmlElement_(_IN scew_element *element, _IN XmlElement *in_element)
{
	int ret = 0;
	XML_Char const *element_key = NULL;
	XML_Char const *contents = NULL;

	unsigned int indent = 0;

	scew_list *list = NULL;


	if (element == NULL || NULL == in_element ||
		0 == strcmp(in_element->element_key, "") ||
		0 == strcmp(in_element->parent_key[0], ""))
	{
		return ret;
	}
	if (NULL == in_element->cur_parent_key) {
		in_element->cur_parent_key = *in_element->parent_key;
		in_element->cur_parent_key_index = 0;
	}

	/* Prints the starting element tag with its attributes. */
	element_key = scew_element_name(element);
	//print_attributes (element);
	contents = scew_element_contents(element);
	// compare key_name is right
	list = scew_element_children(element);
	scew_attribute *attribute = NULL;
	if (0 == strcmp(element_key, in_element->cur_parent_key)) {
		if (-1 == in_element->cur_parent_key_index) {
			ret = 1;
			GetXmlElement_2(element, in_element);
			return ret;
		}
		in_element->cur_parent_key_index++;
		if (0 <= in_element->cur_parent_key_index &&
			0 != strcmp(in_element->parent_key[in_element->cur_parent_key_index], "")
			) {
			in_element->cur_parent_key = in_element->parent_key[in_element->cur_parent_key_index];
		} else {
			if (0 <= in_element->cur_parent_key_index) {
				in_element->cur_parent_key = in_element->element_key;
				in_element->cur_parent_key_index = -1;
			} else {
				ret = 1;
				GetXmlElement_2(element, in_element);
				return ret;
			}
		}
	}

	/**
	* Call print_element function again for each child of the current
	* element.
	*/
	while (list != NULL)
	{
		scew_element *child = (scew_element *)scew_list_data(list);
		ret = GetXmlElement_(child, in_element);
		if (1 == ret) {
			// get content
			//GetXmlElement_2(child, in_element);
			break;
		}
		indent++;
		list = scew_list_next (list);
	}
	
	return ret;
}

void HNProtocol::GetXmlElement_2(_IN scew_element *element, _IN XmlElement *out_element)
{
	char p_in[MAX_ELEMENT_SIZE] = { '\0' };
	char p_out[MAX_ELEMENT_SIZE] = { '\0' };
	XML_Char const *contents = NULL;
	if (NULL == element)
	{
		return ;
	}
	contents = scew_element_contents(element);
	if (NULL != contents) {
		memset(p_in, '\0', MAX_ELEMENT_SIZE);
		memset(p_out, '\0', MAX_ELEMENT_SIZE);
		memcpy(p_in, contents, _MIN(strlen(contents), MAX_ELEMENT_SIZE));
		UTF_8ToGB2312(p_out, p_in, strlen(p_in));

		memset(out_element->element_info, '\0', MAX_ELEMENT_SIZE);
		memcpy(out_element->element_info, p_out, strlen(p_out));
	}
	// key attrib 
	unsigned int n_attribs = scew_element_attribute_count(element);
	unsigned int attribs_count = 0;
	if (0 < n_attribs) {
		scew_list *list = scew_element_attributes (element);
		while (list != NULL)
		{
			// attrib name
			scew_attribute *attribute = (scew_attribute *)scew_list_data(list);
			contents = scew_attribute_name(attribute);
			if (NULL != contents) {
				memset(p_in, '\0', MAX_ELEMENT_SIZE);
				memset(p_out, '\0', MAX_ELEMENT_SIZE);
				memcpy(p_in, contents, _MIN(strlen(contents), MAX_ELEMENT_SIZE));
				UTF_8ToGB2312(p_out, p_in, strlen(p_in));

				memset(out_element->key_attrib[attribs_count], '\0', MAX_ELEMENT_SIZE);
				memcpy(out_element->key_attrib[attribs_count], contents, strlen(contents));
			}
			// attrib info
			if (NULL != contents) {
				contents = scew_attribute_value(attribute);
				memset(p_in, '\0', MAX_ELEMENT_SIZE);
				memset(p_out, '\0', MAX_ELEMENT_SIZE);
				memcpy(p_in, contents, _MIN(strlen(contents), MAX_ELEMENT_SIZE));
				UTF_8ToGB2312(p_out, p_in, strlen(p_in));

				memset(out_element->key_attrib_content[attribs_count], '\0', MAX_ELEMENT_SIZE);
				memcpy(out_element->key_attrib_content[attribs_count], contents, strlen(contents));
			}
			list = scew_list_next (list);
			attribs_count++;
		}
	}
	
}
