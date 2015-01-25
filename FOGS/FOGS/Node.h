#pragma once
#include <map>
#include <list>
#include "Value.h"
#include "Attribute.h"

namespace FOGS
{
	class FOGS_Context;
	
	struct NodeData
	{
		NodeData(FOGS_Context* _context);
		~NodeData();

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
		NodeData* m_Sibling = 0;
		NodeData* m_Nodes = 0;
		NodeData* m_Last = 0;
		unsigned int m_NodesSize = 0;

		AttributeData* m_Attributes = 0;
		AttributeData* m_AttLast = 0;
		unsigned int m_AttrSize = 0;

		bool m_NameEnds = false;
		bool ContextHolder = true;
		ValueData* m_Value = 0;
		FOGS_Context* m_Context = 0;
		NodeData* m_Parent = 0;

		char* m_End = 0;
	};

	extern Allocator<sizeof(NodeData)> NodeAlloc;
	extern Allocator<sizeof(AttributeData)> AttrAlloc;

	class FOGS_Node
	{
	public:
		class Iterator
		{
			NodeData* m_Root;
		public:
			Iterator(NodeData* _root);
			FOGS_Node operator*() const;
			Iterator& operator++();
			bool operator!=(const Iterator& _rval);
		};

		Iterator begin();
		Iterator end();

		FOGS_Node(NodeData* _data);
		
		std::string Name();
		FOGS_Node& Name(const std::string& _val);
		FOGS_Node& Name(const char* _val);

		bool IsNull();
		FOGS_Node Next();
		FOGS_Value Value();

		unsigned int ChildsCount();
		FOGS_Node Childe(const std::string& _key);
		FOGS_Node AppendChild();
		FOGS_Node AppendChild(const char* _name);
		FOGS_Node AppendChild(const std::string& _name);
		std::list<FOGS_Node> Childs();
		std::list<FOGS_Node> Childs(const char* _name);
		std::list<FOGS_Node> Childs(const std::string& _name);
		std::map<std::string, std::list<FOGS_Node>> MapChilds();

		unsigned int AttributesCount();
		FOGS_Attribute Attribute(const std::string& _key);
		FOGS_Attribute AppendAttribute();
		FOGS_Attribute AppendAttribute(const char* _name);
		FOGS_Attribute AppendAttribute(const std::string& _name);
		std::list<FOGS_Attribute> Attributes();
		std::list<FOGS_Attribute> Attributes(const char* _name);
		std::list<FOGS_Attribute> Attributes(const std::string& _name);
		std::map<std::string, std::list<FOGS_Attribute>> MapAttributes();

		operator bool();
		FOGS_Node operator[](const std::string& _key);
		FOGS_Node operator[](const char* _key);
		FOGS_Attribute operator()(const std::string& _key);	

	private:
		NodeData* m_Data = 0;
	};

	FOGS_Node::Iterator begin(FOGS_Node& _val);
	FOGS_Node::Iterator end(FOGS_Node& _val);
}