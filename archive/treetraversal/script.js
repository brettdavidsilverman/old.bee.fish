function Node(input) {

   this.parent = input.parent;
   this.leaf = input.leaf;
 
   if (!isNull(input.nextChildIndex))
      this.nextChildIndex = input.nextChildIndex;
   else
      this.nextChildIndex = 0;
   
   
   if (input.getChildren == undefined)
      this.getChildren = Node.getChildren;
   else
      this.getChildren = input.getChildren;
   
   this.children =
      this.getChildren(this.leaf);

   if (isNull(this.children))
      this.children = [];
      
   if (!isNull(input.parent))
      this.depth = input.parent.depth + 1;
   else
      this.depth = 0;
      
   this.next = function() {
     
      var node = this;

      // search up the tree
      while (
         !node.children ||
         node.nextChildIndex >=
            node.children.length
      ) {
          
          // no children here,
          // remove references and
          // go up a node
          delete node.children;
          var oldNode = node;
          node = node.parent;
          delete oldNode.parent;
          if (!node)
             return null;
      }
      
      // go down to get the next child
      node = new Node(
         {
            parent: node,
            leaf: node.children[node.nextChildIndex++],
            getChildren: node.getChildren
         }
      );
            
      
      return node;

   }
   
   this.nextLeast = function() {
      var node = this;
      
      // if you cant go down...
      if (node.nextChildIndex >= node.children.length) {
      
         // if you cant go up...
         if (isNull(node.parent))
            // finished
            return null;
            
         // if you cant go right...
         if (node.parent.nextChildIndex >=
             node.parent.children.length) {
             // go up
             delete node.children;
             var oldNode = node;
             node = node.parent;
             delete oldNode.parent;

             return node;
         }
         
         // go right
         node = new Node(
            {
               parent: node.parent,
               leaf:
                  node.parent.children[
                     node.parent.nextChildIndex++
                  ],
               getChildren:
                  node.parent.getChildren
            }
         );
         
      }
      
      // while you can go down...
      while (node.nextChildIndex <
             node.children.length) {
         // go down
         node = new Node(
            {
               parent: node,
               leaf:
                  node.children[node.nextChildIndex++],
               getChildren:
                  node.getChildren
            }
         );
      }

      
      return node;
   }
   
   this.setValue = function(value) {
   
      if (Array.isArray(
            this.parent.leaf)
         )
         this.parent.children[
            this.parent.nextChildIndex - 1
         ] = value;
      else {
         var keys =
            Object.keys(
               this.parent.leaf
            );
         
         var key = keys[
            this.parent.nextChildIndex - 1
         ];
         
         this.parent.leaf[key] = value;
      }
      
      this.leaf = value;
   }
}
   

Node.getChildren = 
   function getChildren(leaf) {
      
      if (isNull(leaf))
         return [];
      else if (Array.isArray(leaf))
         return leaf;
      else if (typeof leaf == "object")
         return Object.values(leaf);
      else
         return [];
         
   }

Node.Least = function(input) {
   var root = new Node(input);
   var least = root.nextLeast();
   if (isNull(least))
      least = root;
   return least;
}