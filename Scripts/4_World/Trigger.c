class BubakTrigger extends Trigger
{
    protected string m_TriggerName;
	protected string m_TriggerNotification;
	protected int m_TriggerNotificationTime;
	protected ref TStringArray m_SpawnLocations;
	protected ref TStringArray m_Bubaci;
	protected int m_TriggerDelay;
	protected int m_BubakNum;
	protected int m_LastTriggerTime;
	protected int m_OnlyFillUpToBubaknum;
	
	void SetTriggerName(string name)
	{
		m_TriggerName = name;
	}
	
	string GetTriggerName()
	{
		return m_TriggerName;
	}
	
	void SetTriggerNotification(string notification)
	{
		m_TriggerNotification = notification;
	}
	
	string GetTriggerNotification()
	{
		return m_TriggerNotification;
	}
	
	void SetTriggerNotificationTime(int notificationtime)
	{
		m_TriggerNotificationTime = notificationtime;
	}
	
	int GetTriggerNotificationTime()
	{
		return m_TriggerNotificationTime;
	}
	
	void SetSpawnLocations(TStringArray spawnloc)
	{
		m_SpawnLocations = spawnloc;
	}
	
	TStringArray GetSpawnLocations()
	{
		return m_SpawnLocations;
	}
	
	void SetBubaci(TStringArray bubaci)
	{
		m_Bubaci = bubaci;
	}
	
	TStringArray GetBubaci()
	{
		return m_Bubaci;
	}
	
	void SetTriggerDelay(int delay)
	{
		m_TriggerDelay = delay;
	}
	
	void SetLastTriggerTime(int time)
	{
		m_LastTriggerTime = time;
	}
	
	void SetBubakNum(int bubaknum)
	{
		m_BubakNum = bubaknum;
	}

	void SetOnlyFillUpToBubaknum(int onlyFillUpToBubaknum)
	{
		m_OnlyFillUpToBubaknum = onlyFillUpToBubaknum;
	}

	bool CanTriggerAction(int time)
	{
		if (time > m_LastTriggerTime+m_TriggerDelay)
		{
			m_LastTriggerTime = time;
			return true;
		}
			
		return false;
	}
	
	
	void OnEnter(Object obj)
    {
		SPBLogger.GetInstance().Log( "SpawnerBubaku ENTER" );

		if (obj.IsMan())
		{
			// porovnat s ulozenym casem tiku, kdyz je vetsi nez cooldown ulozit novy a povolit akci
			//GetGame().CreateObject("Seachest", obj.GetPosition() );
			//GetGame().CreateObject("ZmbM_ClerkFat_White", obj.GetPosition(), false, true, true );
			SPBLogger.GetInstance().Log( "Triggered " + GetTriggerName() + " time " + GetGame().GetTime()/1000);
			if (CanTriggerAction(GetGame().GetTime()/1000))
			{
				if (PlayerBase.Cast(obj).GetIdentity())
				{
					if (PlayerBase.Cast(obj).GetIdentity().GetName())
					{	
						SPBLogger.GetInstance().Log("Can trigger action " + GetGame().GetTime()/1000 + " last " + m_LastTriggerTime + " trigger delay " + m_TriggerDelay);
						SetLastTriggerTime(GetGame().GetTime()/1000);
						SpawniBubaky();
						if (GetTriggerNotification() != "")
						{
							NotificationSystem.SendNotificationToPlayerExtended(Man.Cast(obj), GetTriggerNotificationTime(), GetTriggerName(), GetTriggerNotification());
						}
						SPBLogger.GetInstance().Log( PlayerBase.Cast(obj).GetIdentity().GetName() + " triggered " + GetTriggerName(), SPBLogger.LOGLEVEL_CRITICAL);
					}
				}
			}
			else
			{
				SPBLogger.GetInstance().Log("Cannot trigger action it was called at " +  m_LastTriggerTime );
			}
		}
		
    }

    void OnLeave(Object obj)
    {    
		SPBLogger.GetInstance().Log( "SpawnerBubaku EXIT" );
    }

	/*
    override void EOnInit(IEntity other, int extra)
    {
        const vector mins = "-1 -0.2 -1";
        const vector maxs = "1 1 1";
        SetExtents(mins, maxs);
    }
	*/

	int SpawnerBubaku_GetActiveObjectsNum()
	{
		TIntArray spawned_instances = SpawnerBubaku.GetInstance().GetSpawnedInstances(GetID());
		if(spawned_instances) return spawned_instances.Count();
		return 0;
	}

	Object SpawnerBubaku_CreateObject(string type, vector pos, bool create_local = false, bool init_ai = false, bool create_physics = true)
	{
		auto newObject = GetGame().CreateObject(type, pos, create_local, init_ai, create_physics);
		SPBLogger.GetInstance().Log("Created bubak: " + newObject.GetID());
		SPBLogger.GetInstance().Log("pos: " + pos.ToString());

		if(m_OnlyFillUpToBubaknum && newObject)
		{
			ZombieBase zombie = ZombieBase.Cast(newObject);
			if(zombie)
			{
				zombie.SetTriggerId(GetID());
				SpawnerBubaku.GetInstance().AddSpawnedInstance(GetID(), zombie.GetID());
			}
		}
		
		return newObject;
	}

	void SpawniBubaky()
	{
		vector randvec, randompos;
		string randvecstr;
		float yko;
		int rndnum,i;
		int j = 0;
		bool rotated;
		string posrot, pos,ori;
		TStringArray loc;

		SPBLogger.GetInstance().Log("m_OnlyFillUpToBubaknum: " + m_OnlyFillUpToBubaknum);
		SPBLogger.GetInstance().Log("m_BubakNum: " + m_BubakNum);
		SPBLogger.GetInstance().Log("m_SpawnLocations.Count(): " + m_SpawnLocations.Count());
		SPBLogger.GetInstance().Log("SpawnerBubaku_GetActiveObjectsNum(): " + SpawnerBubaku_GetActiveObjectsNum());

		int create_bubaks = m_BubakNum - SpawnerBubaku_GetActiveObjectsNum();
		
		SPBLogger.GetInstance().Log("Need to create: " + create_bubaks);

		if (create_bubaks < m_SpawnLocations.Count())
		{
			//ramdom select spawn pos
			TStringArray positions = new TStringArray();
			positions.Copy(m_SpawnLocations);
			for ( i=0; i < create_bubaks; i++)
			{
				rndnum = Math.RandomIntInclusive(0, positions.Count() - 1);
				SPBLogger.GetInstance().Log("random position index: " + rndnum);

				rotated = false;
				pos = positions.Get(rndnum);
				ori = "0 0 0";
				if (positions.Get(rndnum).Contains("|"))
				{
					posrot = positions.Get(rndnum);
					loc = new TStringArray;
					posrot.Split( "|", loc );
					pos = loc.Get(0);
					ori = loc.Get(1);
					rotated = true;
				}
				
				auto object1 = SpawnerBubaku_CreateObject(GetBubaci().GetRandomElement(), pos.ToVector() ,false,true, true);
				if (rotated)
				{
					object1.SetOrientation(ori.ToVector());
				}
				positions.RemoveOrdered(rndnum);
				
			}
			
		} 
		else 
		{
			for ( i=0; i < create_bubaks; i++)
			{
				/*
				randompos = m_SpawnLocations.GetRandomElement().ToVector();
				randvecstr = Math.RandomFloatInclusive(-0.5, 0.5).ToString()+ " 0 " + Math.RandomFloatInclusive(-0.5, 0.5).ToString();
				randvec = randompos+randvecstr.ToVector();
				GetGame().CreateObject(GetBubaci().GetRandomElement(),randvec,false,true, true);
				*/
				rotated = false;
				pos = m_SpawnLocations.Get(j);
				SPBLogger.GetInstance().Log("position index: " + j);

				ori = "0 0 0";
				if (m_SpawnLocations.Get(j).Contains("|"))
				{
					posrot = m_SpawnLocations.Get(j);
					loc = new TStringArray;
					posrot.Split( "|", loc );
					pos = loc.Get(0);
					ori = loc.Get(1);
					rotated = true;
				}
				
				auto object2 = SpawnerBubaku_CreateObject(GetBubaci().GetRandomElement(), pos.ToVector() ,false,true, true);
				if (rotated)
				{
					object2.SetOrientation(ori.ToVector());
				}
				j++;
				if (j == m_SpawnLocations.Count())
				{
					j = 0;
				}
			}
		}	
	}
};
