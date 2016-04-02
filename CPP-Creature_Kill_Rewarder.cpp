// By slp13at420 of EmuDevs.com

#include "Player.h"
#include "PlayerAI\PlayerAI.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "WorldSession.h"

std::string DB_COLUMN = "reward_count"; // this is the name of the element in auth.account you added to store this value. use default '0' so it wont need another block for OnAccountCreation.
std::unordered_map<uint32, uint32>CKR;

void StoreCredit(uint32 id, uint32 count)
{
	LoginDatabase.PExecute("UPDATE account SET `%s` = '%u' WHERE `id` = '%u';", DB_COLUMN.c_str(), count, id);
};

class KillCreatureCredit_Account_Events : public AccountScript
{
public: KillCreatureCredit_Account_Events() : AccountScript("KillCreatureCredit_Account_Events"){ };

	virtual void OnAccountLogout(uint32 accountId)
	{
		WorldDatabase.PExecute("REPLACE INTO account SET `%s`='%u';", DB_COLUMN.c_str(), CKR[accountId]);

		CKR.erase(accountId);
	};

	virtual void OnAccountLogin(uint32 accountId)
	{
		if (accountId > 0)
		{
			QueryResult CKRPlayerQery = LoginDatabase.PQuery("SELECT `%s` FROM `account` WHERE `id` = '%u';", DB_COLUMN.c_str(), accountId);

			if (CKRPlayerQery)
			{
				do
				{
					Field* fields = CKRPlayerQery->Fetch();
					uint32 Reward_Count = fields[0].GetUInt32();

					CKR[accountId] = Reward_Count;

				} while (CKRPlayerQery->NextRow());
			}
		}
	}
};

class KillCreatureCredit : public PlayerScript
{
public: KillCreatureCredit() : PlayerScript("KillCreatureCredit"){ }

	void OnCreatureKill(Player* killer, Creature* killed)
	{
		StoreCredit(killer->GetSession()->GetAccountId(), 2);
	}
};

void AddSC_KillCreatureRewarder()
{
	new KillCreatureCredit_Account_Events;
	new KillCreatureCredit;
}
