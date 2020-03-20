function BinaryTree(left, right)
{
    var binaryTree = this;
    this.left = left;
    this.right = right;
    this.Write = function() {
        this.binaryTree = binaryTree;
        this.write = function(bit) {
            if (bit == 0) {
                if (this.binaryTree.left == undefined)
                    this.binaryTree.left = new BinaryTree();
                this.binaryTree = this.binaryTree.left;
            }
            else {
                if (this.binaryTree.right == undefined)
                    this.binaryTree.right = new BinaryTree();
                this.binaryTree = this.binaryTree.right;
            }
        }
    }

    this.read = function(stream) {

        read(stream, this);

        function read(stream, binaryTree) {

            if (binaryTree == undefined) {
                stream.write(0);
                return;
            }

            if (binaryTree.left == undefined && binaryTree.right == undefined) {
                if (!stream.write(1, 0))
                    stream.write(0);
                return;
            }

            stream.write(1);

            read(stream, binaryTree.left);
            read(stream, binaryTree.right);
        }
    }

    this.Write = function() {
        var path = new Array();
        path.push(binaryTree);
        this.isEnd = (binaryTree.left == binaryTree.right == undefined);
        this.peek = (binaryTree.left != undefined);
        this.next = function() {
            var binaryTree = path[path.length - 1];
            var next;
            if (binaryTree.left != undefined) {
                next = 0;
                binaryTree = binaryTree.left;
            } else {
                while (path.length > 0
                      && binaryTree.right == undefined) {                        next = 1;
                    binaryTree = path.pop();
                }
                if (binaryTree.right != undefined) {
                    next = 1;
                    binaryTree = binaryTree.right;
                }
                else {
                    binaryTree = undefined;
                    next = undefined;
                }
            }
            if (binaryTree != undefined)
            {
                path.push(binaryTree);
                if (binaryTree.left != undefined)
                    this.peek = 0;
                else
                    this.peek = 1;
            }

            if (next == undefined) {
                this.isEnd = true;
                this.peek = undefined;
            }
            else
                this.isEnd = false;

            return next;
        }
    }
}

BinaryTree.Write = function() {
    this.values = new Array();
    var values = this.values;
    var left = undefined;
    var parents = new Array();
    var binaryTree;
    this.chain = function(chain) {
        next = chain;
    }
    var next;
    function add() {
        if (next != undefined)
            next(parent);
        else
            values.push(parent);
        left = undefined;
        parent = undefined;
    }
    this.writeToLeft = function(bit, next) {
        if (bit == 0) {
            if (binaryTree == undefined)
                add(undefined);
            else
                this.write = this.writeToRight;
            return false;
        }
        if (bit == 1 && nextBit == 0) {
            binaryTree.left = new BinaryTree();
            this.write = writeToRight;
            return true;
        }
        if (bit == 1) {
            binaryTree.left = new BinaryTree();
            parents.push(binaryTree);
            binaryTree = binaryTree.left;
            return true;
        }
    }

    this.writeToRight = function(bit, next) {
        if (bit == 0) {
            bin
        }
    }

    this.write = function(bit, next) {
        if (bit == 0) {
            if (parent == undefined) {
                add();
                return false;
            }
            else if (left)
                left = false;
            else {
                if (parent.parent == undefined)
                    add();
                else
                    parent = parent.parent;
            }
            return false;
        }

        if (next == 0) {
            if (parent == undefined)
                parent = new BinaryTree();
            else if (left) {
                parent.left = new BinaryTree();
                left = false;
            }
            else {
                parent.right = new BinaryTree();
                left = false;
                if (parent.parent == undefined)
                    add();
                else
                    parent = parent.parent;
            }
            return true;
        }

        if (parent == undefined)
            parent = new BinaryTree();
        else if (left) {
            parent.left = new BinaryTree();
            parent.left.parent = parent;
            parent = parent.left;
        }
        else {
            parent.right = new BinaryTree();
            parent.right.parent = parent;
            parent = parent.right;
        }
        left = true;
        return false;
    }


}
