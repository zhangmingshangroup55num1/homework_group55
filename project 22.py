class TrieNode:
    def __init__(self):
        self.children = {}
        self.is_end_of_word = False
        self.value = None

class MerklePatriciaTree:
    def __init__(self):
        self.root = TrieNode()

    def insert(self, key, value):
        node = self.root
        for char in key:
            if char not in node.children:
                node.children[char] = TrieNode()
            node = node.children[char]
        node.is_end_of_word = True
        node.value = value

    def search(self, key):
        node = self.root
        for char in key:
            if char not in node.children:
                return None
            node = node.children[char]
        if node.is_end_of_word:
            return node.value
        else:
            return None

    def get_root_hash(self):
        return self._get_node_hash(self.root)

    def _get_node_hash(self, node):
        if node.is_end_of_word:
            return hash(node.value)
        else:
            children_hashes = []
            for char, child in node.children.items():
                child_hash = self._get_node_hash(child)
                children_hashes.append((char, child_hash))
            return hash(children_hashes)
tree = MerklePatriciaTree()

tree.insert("amy", 1)
tree.insert("daming", 2)
tree.insert("hh", 3)

print(tree.search("amy"))  
print(tree.search("hh")) 
print(tree.search("daming")) 
print(tree.search("xx")) 

