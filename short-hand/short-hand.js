class Shorthand {
   static #stack = [];
   
   static HUMAN = "human";
   static POINTERS = "pointers";
   static FULL = "full";
   
   static get current() {
      return Shorthand.#stack[
         Shorthand.#stack.length - 1
      ];
   }
   
   static get type() {
      return Shorthand.current.type;
   }
   
   static get map() {
      return Shorthand.current.map;
   }
   
   static push(shortHand) {
      Shorthand.#stack.push(shortHand);
   }
   
   static pop() {
      Shorthand.#stack.pop();
   }
   
   constructor(type) {
      Shorthand.push(
         {
            type,
            map: new Map()
         }
      );
   }
   
}

new Shorthand(Shorthand.HUMAN);