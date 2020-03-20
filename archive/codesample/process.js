const EXIT = {};

function process(block) {
   while (block != EXIT) {
      block = blocks[block]();
   }
}
