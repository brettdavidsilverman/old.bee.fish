var stack = [];

var procedures = [
   function add(x,y) {
      output = x + y;
      return undefined;
   },
   function multiply(x, y) {
      output = x * y;
      return 2;
   },
   function check(x, y) {
       output = (x * y) == multiply(x,y);
      return undefined;
   }
];

function run(procedureId) {
   stack.push(procedureId);

   while (stack.length) {
      procedureId = stack.pop();

      var procedure = procedures[procedureId];
      var next = procedure();

      if (next == undefined)
         continue;

      for (var index in next) {
         procedureId = next[index];
         stack.push(procedureId);
      }
   
   }
}