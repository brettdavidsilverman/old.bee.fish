class ShortHand {
   static #stack = [];
   static HUMAN = "human";
   static STATE = "state";
   static FULL = "full";
   
   static get current() {
      return ShortHand.#stack[
         ShortHand.#stack.length - 1
      ];
   }
   
   static push(shortHand) {
      ShortHand.#stack.push(shortHand);
   }
   
   static pop() {
      ShortHand.#stack.pop();
   }
   
}

ShortHand.push(ShortHand.HUMAN);