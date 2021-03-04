#ifndef BEE_FISH_JSON__SET_H
#define BEE_FISH_JSON__SET_H
#include <ostream>
#include <vector>
#include <optional>
#include "../parser/parser.h"
#include "blank-space.h"

using namespace std;

namespace bee::fish::json
{

	   class Set : public Match
		          {
				     public:
					       
					           Set( MatchPtr openBrace,
								              MatchPtr item,
									                 MatchPtr seperator,
											            MatchPtr closeBrace
												          ) : Match()
													            {
															             MatchPtr OpenBrace =
																	                 openBrace and ~BlankSpace;
																              
																              MatchPtr Seperator =
																		                  ~BlankSpace and
																				              seperator and
																					                  ~BlankSpace;
																	               
																	               MatchPtr CloseBrace =
																			                   ~BlankSpace and closeBrace;
																		                
																		                class Item : public Match
																					              {
																							               public:
																									                   shared_ptr<Set> _set;
																											               
																											            public:
																											               Item(shared_ptr<Set> set) 
																													                      Match(),
																													                      _set(set)
																																                  {
																																			                 setMatch(item);
																																					             }
																												                   
																												                   virtual void success()
																															               {
																																	                      Match::success();
																																			                     _set->matchedSetItem(this);
																																					                 }
																														            };
																				         
																				         MatchPtr Set =
																						             OpenBrace and
																							                 Optional(
																											                (new Item(this)) and
																													               Repeat(
																															                         Seperator and
																																		                   (new Item(this)),
																																				                     0
																																						                    )
																														                   ) and
																									             CloseBrace;
																					          
																					          setMatch(Set);
																						        }
						         
						         void callMatched(MatchPtr matched)
								       {
									                cerr << *this;
											         this->matchedSetItem(matched);
												       }
							       
							       Set(const Set& source) : Match(source)
											      {
												               
												            }
							             
							             virtual MatchPtrBase copy() const
									           {
											            return make_shared<Set>(*this);
												          }
								           

								           virtual void matchedSetItem(MatchPtr item)
										         {
												       }
									         
									      };
	         
}

#endif

