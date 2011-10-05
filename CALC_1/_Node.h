// Nodes generator
// Define these and include
// avaliable function to use:
// std::string getToken() const
// int childrenCount() const
//
// NODE_NAME -- class name ~PlusNode
// NODE_XML_START -- xml representing the start of the node ~ "<plus />"
// NODE_XML_END -- xml representing the end of the node ""
//

class NODE_NAME: public Node {
	private:
	public:
		NODE_NAME(string token = ""):
		Node(token)
		{}
		
		
		string XMLStart() const {
			string xml = NODE_XML_START;
			return xml;
		}
		
		string XMLEnd() const {
			string xml = NODE_XML_END;
			return xml;
		}
};
