function BinaryTree(parent, left, right)
{
    this.left = left;
    this.right = right;
    this.parent = parent;

}

BinaryTree.prototype.openRead = function() {

    return new BinaryTree.ReadStream(this);

}

BinaryTree.ReadStream = function(binaryTree) {

    this.binaryTree = binaryTree;
    this.path = new BitStream();
    this.start = this.binaryTree;
    this.pathReadStream = undefined;
    this.isEnd = (this.binaryTree.left == undefined && this.binaryTree.right == undefined);
}

BinaryTree.ReadStream.prototype.read = function() {
    var read;
    if (this.pathReadStream != undefined) {
        // Read from existing path
        read = this.pathReadStream.read();
        if (read == 0)
            this.binaryTree = this.binaryTree.left;
        else
            this.binaryTree = this.binaryTree.right;

        if (this.pathReadStream.isEnd) {
            if (this.binaryTree.left != undefined) {
                this.isEnd = false;
                this.peek = 0;
            }
            else if (this.binaryTree.right != undefined) {
                this.isEnd = false;
                this.peek = 1;
            }
            else {
                var binaryTree = this.binaryTree;
                var wasRight = false;
                while(binaryTree != this.start &&
                        ((binaryTree == binaryTree.parent.left
                        && binaryTree.parent.right == undefined)
                        || (binaryTree == binaryTree.parent.right))) {
                    wasRight = (binaryTree.parent.right == binaryTree);
                    binaryTree = binaryTree.parent;
                }
                if (binaryTree == this.start && wasRight || this.start.right == undefined)
                    this.isEnd = true;
                else
                    this.isEnd = false;
            }
            this.pathReadStream = undefined;
        }
        else {
            this.isEnd = false;
            this.peek = this.pathReadStream.peek;
        }
        return read;
    }

    if (this.binaryTree.left != undefined) {
        read = 0;
        this.binaryTree = this.binaryTree.left;
    }
    else if (this.binaryTree.right != undefined) {
        read = 1;
        this.binaryTree = this.binaryTree.right;
    }
    else
        return undefined;

    this.path.write(read);
    if (this.binaryTree.left != undefined) {
        this.peek = 0;
        this.isEnd = false;
    }
    else if (this.binaryTree.right != undefined) {
        this.peek = 1;
        this.isEnd = false;
    }
    else
        this.isEnd = true;

    return read;

}

BinaryTree.ReadStream.prototype.next = function() {
    //document.write(this.path.array.join(",") + "::");
    var binaryTree = this.binaryTree;
    var wasRight = false;
    while(binaryTree != this.start && (
            (binaryTree == binaryTree.parent.left
            && binaryTree.parent.right == undefined)
            || (binaryTree == binaryTree.parent.right))) {
        wasRight = (binaryTree.parent.right == binaryTree);
        binaryTree = binaryTree.parent;
        this.path.pop();
    }

    if (binaryTree == this.start) {
        if (wasRight || this.start.right == undefined) {
            this.isEnd = true;
            return false;
        }
    }
    else
        this.path.pop();
    this.binaryTree = this.start;
    this.path.write(1);
    //document.write(this.path.array.join(",") + "</br>");
    this.pathReadStream = this.path.openRead();
    this.peek = this.pathReadStream.peek;
    this.isEnd = false;
    return true;
}


BinaryTree.prototype.openWrite = function() {
    return new BinaryTree.WriteStream(this);
}

BinaryTree.WriteStream = function(binaryTree) {
    this.start = binaryTree;
    this.binaryTree = binaryTree;
}

BinaryTree.WriteStream.prototype.next = function() {
    this.binaryTree = this.start;
}

BinaryTree.WriteStream.prototype.write = function(bit) {

    if (bit == 0) {
        if (this.binaryTree.left == undefined)
            this.binaryTree.left = new BinaryTree(this.binaryTree);

        this.binaryTree = this.binaryTree.left;
    }
    else {
        if (this.binaryTree.right == undefined)
            this.binaryTree.right = new BinaryTree(this.binaryTree);

        this.binaryTree = this.binaryTree.right;
    }
}

BinaryTree.read = function(stream) {

    return read(stream);

    function read(stream, parent) {

        if (stream.read() == 0)
            return undefined;

        var binaryTree = new BinaryTree(parent);

        binaryTree.left = read(stream, binaryTree);
        binaryTree.right = read(stream, binaryTree);

        return binaryTree;
    }
}

BinaryTree.prototype.write = function(stream) {

    write(this, stream);

    function write(binaryTree, stream) {
        if (binaryTree == undefined)
            stream.write(0);
        else {
            stream.write(1);
            write(binaryTree.left, stream);
            write(binaryTree.right, stream);
        }
    }
}

