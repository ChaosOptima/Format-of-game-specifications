#pragma once
#include <map>
#include <list>
#include "Value.h"
#include "Attribute.h"

namespace FOGS
{
	class FOGS_Context;
	
	struct Node_impl
	{
		Node_impl(FOGS_Context* _context);
		~Node_impl();

		void ReadNodes();
		void ReadName();
		void ReadAttributes();
		void NameEnds();
		bool IsUnexpected(char* _char);
		bool NodesReader(char* _char);
		bool AttributesReader(char* _char);
		bool NameReader(char* _char);

		void AppendNode();

		char* m_Name = 0;
		Node_impl* m_Sibling = 0;
		Node_impl* m_Nodes = 0;
		Node_impl* m_Last = 0;
		unsigned int m_NodesSize = 0;

		Attribute_impl* m_Attributes = 0;
		Attribute_impl* m_AttLast = 0;
		unsigned int m_AttrSize = 0;

		bool m_NameEnds = false;
		bool ContextHolder = true;
		ValueData_impl* m_Value = 0;
		FOGS_Context* m_Context = 0;
		Node_impl* m_Parent = 0;

		char* m_End = 0;
	};

	extern Allocator<sizeof(Node_impl)> NodeAlloc;
	extern Allocator<sizeof(Attribute_impl)> AttrAlloc;

	class Node
	{
	public:
		class Iterator
		{
			Node_impl* m_Root;
		public:
			Iterator(Node_impl* _root);
			Node operator*() const;
			Iterator& operator++();
			bool operator!=(const Iterator& _rval);
		};

		Iterator begin();
		Iterator end();

		Node(Node_impl* _data);
		
		std::string Name();
		Node& Name(const std::string& _val);
		Node& Name(const char* _val);

		bool IsNull();
		Node Next();
		ValueData Value();

		unsigned int ChildsCount();
		Node Childe(const std::string& _key);
		Node AppendChild();
		Node AppendChild(const char* _name);
		Node AppendChild(const std::string& _name);
		std::list<Node> Childs();
		std::list<Node> Childs(const char* _name);
		std::list<Node> Childs(const std::string& _name);
		std::map<std::string, std::list<Node>> MapChilds();

		unsigned int AttributesCount();
		Attribute AttributeAt(const std::string& _key);
		Attribute AppendAttribute();
		Attribute AppendAttribute(const char* _name);
		Attribute AppendAttribute(const std::string& _name);
		std::list<Attribute> Attributes();
		std::list<Attribute> Attributes(const char* _name);
		std::list<Attribute> Attributes(const std::string& _name);
		std::map<std::string, std::list<Attribute>> MapAttributes();

		operator bool();
		Node operator[](const std::string& _key);
		Node operator[](const char* _key);
		Attribute operator()(const std::string& _key);	

	private:
		Node_impl* m_Data = 0;
	};

	Node::Iterator begin(Node& _val);
	Node::Iterator end(Node& _val);
}