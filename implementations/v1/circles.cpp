/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <circles.hpp>

/**
 *  The init() and apply() methods must have C calling convention so that the blockchain can lookup and
 *  call these methods.
 */

namespace circles {
    void createAccount(uint64_t account)
    {

    }
    void claimTokens(uint64_t account)
    {
        require_auth(account);
        last_claim query;
        query.account = account;
        last_claims::get(query);
        if(query.last_claim == 0){
            eosio::print("Created account at: ", now());
        }else{
            uint64_t diff = now() - query.last_claim;
            eosio::print("Granting tokens: ", diff);
            circles::addTokens(account, account, diff);
        }

        last_claim lc { account, now() };
        last_claims::store(lc);

    }
    void addTokens(uint64_t account, uint64_t token, uint64_t amount){
      token query;
      //important info: when you want to access/modify witin the users scope it has to be specified in the message --scope account
      //so if we want to check if a user has a token or modify any info regarding the token
      //we have to first query it witnin the users scope
        query.token_name = token; // i specified it as account_name in the struct but that this is only to explain scopes so it doesnt really matter
        bool get_token = TokenTable::get(query, account); // now here is the important part
        //we query the token table with the primary_key token_name within the scope of the account
        //if we would just use the first parameter TokenTable::get(query); it would query the default scope @ default_scope_set_when_you_defined_the_table.tokens.token_name
        //but with the account scpecified as scope we query @ account_name.tokens.token_name
        query.balance = query.balance + amount;
        if(get_token == true) {
          //token is there so we can modify and update it
          //important part is that we again specify the account as scope
          TokenTable::update(query, account);
        } else {
          //token isnt there so we can add it
          //important part is that we again specify the account as scope
          TokenTable::store(query, account);
        }
        assert(add_token, "Error adding or modifying token");
    }
    void trust(trust_relation relation)
    {
          require_auth(relation.trustor);
          eosio::print("TODO: Let ", eosio::name(relation.trustor),  " trust account ", eosio::name(relation.trustee), " \n");
    }
    void untrust(trust_relation relation)
    {
          require_auth(relation.trustor);
    }
    void apply_exchange(exchange exchange_msg)
    {
        require_auth(exchange_msg.from);
        eosio::print( "Transfer ", exchange_msg.amount, " from ", eosio::name(exchange_msg.from),"(", eosio::name(exchange_msg.from_currency), ") to ", eosio::name(exchange_msg.to), "(", eosio::name(exchange_msg.to_currency),  ")\n" );
    }

}

using namespace circles;

extern "C" {

    void init()  {
       eosio::print( "Initializing Circles V1 contract\n" );
    }

/// The apply method implements the dispatch of events to this contract
    void apply( uint64_t code, uint64_t action )
    {
         if(action == N(newaccount))
	 {
             eosio::print("TODO: Create new account \n");
         }
	 else if(action == N(claim))
	 {
	     eosio::print("TODO: CLaim currency \n");
             account message;
	     static_assert(sizeof(message) == 1*sizeof(uint64_t), "unexpected padding");
	     auto read = read_message(&message, sizeof(message));
	     assert(read == sizeof(message), "message too short");
             circles::claimTokens(message.account);
         }
	 else if(action == N(trust))
	 {
             trust_relation message;
	     static_assert(sizeof(message) == 2*sizeof(uint64_t), "unexpected padding");
	     auto read = read_message(&message, sizeof(message));
	     assert(read == sizeof(message), "message too short");
             circles::trust(message);
         }
	 else if(action == N(untrust))
	 {
             eosio::print("TODO: UNtrust account \n");
         }
	 else if(action == N(exchange))
	 {
             exchange message;
             eosio::print("TODO: Exchange currency \n");
             static_assert(sizeof(message) == 5*sizeof(uint64_t), "unexpected padding");
	     auto read = read_message(&message, sizeof(message));
             assert(read == sizeof(message), "message too short");
	     circles::apply_exchange(message);
	 }
         eosio::print( "Received: ", eosio::name(code), "->", eosio::name(action), "\n" );
    }

} // extern "C"
