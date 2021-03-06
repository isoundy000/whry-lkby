
#include "UpGradeLogic.h"

void Msg(const char *lpszFormat, ...)
{
	va_list pArg;
	va_start(pArg, lpszFormat);
	char szMessage[2048] = { 0 };
#ifdef WIN32
	_vsnprintf(szMessage, 2047, lpszFormat, pArg);
#else
	vsnprintf(szMessage, 2047, lpszFormat, pArg);
#endif
	va_end(pArg);
	OutputDebugString(szMessage);
	return;
}

//构造函数
CUpGradeGameLogic::CUpGradeGameLogic(void)
{
	//m_iNTNum=0;
	//m_iNTHuaKind=UG_ERROR_HUA;
	m_dwCardShape=0;
	m_iStation[4] = 500;
	for (int i=0;i<4;i++)
		m_iStation[i]=100*i;
	
}


//获取扑克花色
BYTE CUpGradeGameLogic::GetCardHuaKind(BYTE iCard)
{ 
	int iHuaKind=(iCard&UG_HUA_MASK);
	
	return iHuaKind; 
}

//获取扑克大小 （2 - 18 ， 15 以上是主牌 ： 2 - 21 ， 15 以上是主）
int CUpGradeGameLogic::GetCardBulk(BYTE iCard, BOOL bExtVol)
{
	if ((iCard==0x4E)||(iCard==0x4F))
	{
		return bExtVol?(iCard-14):(iCard-62);	//大小鬼64+14-62=16			只返回大小猫的值
	}

	int iCardNum=GetCardNum(iCard);
	int iHuaKind=GetCardHuaKind(iCard);

//	if (iCardNum==2)		
//	{
//		if(bExtVol)		
//			return ((iHuaKind>>4)+(15*4));
//		else
//			return 15;
//	}
	return ((bExtVol)?((iHuaKind>>4)+(iCardNum*4)):(iCardNum));

}

//排列扑克
BOOL CUpGradeGameLogic::SortCard(BYTE iCardList[], BYTE bUp[], int iCardCount)
{
	BOOL bSorted=TRUE,bTempUp;
	int iTemp,iLast=iCardCount-1,iStationVol[45];

	//获取位置数值
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i]=GetCardBulk(iCardList[i],TRUE);
		///if (iStationVol[i]>=15) iStationVol[i]+=m_iStation[4];
		///else iStationVol[i]+=m_iStation[GetCardHuaKind(iCardList[i],FALSE)>>4];
	}

	//排序操作
	do
	{
		bSorted=TRUE;
		for (int i=0;i<iLast;i++)
		{
			if (iStationVol[i]<iStationVol[i+1])
			{	
				//交换位置
				iTemp=iCardList[i];
				iCardList[i]=iCardList[i+1];
				iCardList[i+1]=iTemp;
				iTemp=iStationVol[i];
				iStationVol[i]=iStationVol[i+1];
				iStationVol[i+1]=iTemp;
				if (bUp!=NULL)
				{
					bTempUp=bUp[i];
					bUp[i]=bUp[i+1];
					bUp[i+1]=bTempUp;
				}
				bSorted=FALSE;
			}	
		}
		iLast--;
	} while(!bSorted);
		
	return TRUE;
}


/*
//重新排序
BOOL CUpGradeGameLogic::ReSortCard(BYTE iCardList[], int iCardCount)
{
	CPtrList ptrList;

	for (int i = 0;i < iCardCount;i++)
	{	
		bool bFind = false;
		int pd = GetCardNum(iCardList[i]);
		if (pd == 16) pd = 17;	///大王 
		if (pd == 15) pd = 16;	///小王
		if (pd == 2)  pd = 15;
		for (POSITION pos = ptrList.GetHeadPosition();pos != NULL;)
		{
			PSTypeNode *pn = (PSTypeNode *) ptrList.GetNext(pos);
			if (!pn) continue;
			/////ASSERT(pn);
			if (pn->mps == pd)
			{
				pn->mcount++;
				bFind = true;
				break;
			}
		}
		if (!bFind)
		{
			PSTypeNode *pn = new PSTypeNode();
			pn->mps = pd;
			pn->mcount = 1;
			ptrList.AddTail(pn);
		}
	}

	///sort
	CPtrList tmpList;
	for (POSITION pos = ptrList.GetHeadPosition();pos != NULL;)
	{
		PSTypeNode *pn = (PSTypeNode *) ptrList.GetNext(pos);
   		if (!pn) continue;
		////ASSERT(pn);
		PSTypeNode *pii = new PSTypeNode();
		pii->mcount = pn->mcount;
		pii->mps = pn->mps;
		if (tmpList.IsEmpty())
		{
			tmpList.AddTail(pii);
			continue;
		}
		
		bool bFind = false;
		for (POSITION pos1 = tmpList.GetHeadPosition();pos1 != NULL;)
		{
			POSITION tp = pos1;
			PSTypeNode *pi = (PSTypeNode *) tmpList.GetNext(pos1);
			if (!pi) continue;
			if (pii->mcount > pi->mcount || (pii->mcount == pi->mcount && pii->mps > pi->mps))
			{
				tmpList.InsertBefore(tp, pii);
				bFind = true;
				break;
			}
		}

		if (!bFind)
		{
			tmpList.AddTail(pii);
		}
	}

	BYTE iTempCard[39];
	int iCnt = 0;

	for (POSITION pos = tmpList.GetHeadPosition();pos != NULL;)
	{
		PSTypeNode *pn = (PSTypeNode *) tmpList.GetNext(pos);
		if (!pn) continue;
		ASSERT(pn);
		for (int i = 0;i < iCardCount;i++)
		{
			int pd = GetCardNum(iCardList[i]);
			if (pd == 16) pd = 17;	///大王 
			if (pd == 15) pd = 16;	///小王
			if (pd == 2)  pd = 15;
			if (pd == pn->mps)
			{
				iTempCard[iCnt++] = iCardList[i];
			}
		}
	}
	
	while (!ptrList.IsEmpty())
	{
		PSTypeNode *ps = (PSTypeNode *) ptrList.RemoveHead();
		delete ps;
	}
	ptrList.RemoveAll();

	while (!tmpList.IsEmpty())
	{
		PSTypeNode *ps = (PSTypeNode *) tmpList.RemoveHead();
		delete ps;
	}
	tmpList.RemoveAll();

	for (int i = 0;i < iCardCount;i++)
	{
		iCardList[i] = iTempCard[i];
	}
	
	return TRUE;
}
*/
//重新排序
BOOL CUpGradeGameLogic::ReSortCard(BYTE iCardList[], int iCardCount)
{
	SortCard(iCardList,NULL,iCardCount);
	//====按牌形排大小
	int iStationVol[45];
	for (int i=0;i<iCardCount;i++)
	{
		iStationVol[i]=GetCardBulk(iCardList[i],false);
	}

	int Start=0;
	int j,step;
	BYTE CardTemp[8];					//用来保存要移位的牌形
	int CardTempVal[8];					//用来保存移位的牌面值
	for(int i=8;i>1;i--)				//在数组中找一个连续i张相同的值
	{
		for(j=Start;j<iCardCount;j++)
		{
			CardTemp[0]=iCardList[j];			//保存当前i个数组相同的值
			CardTempVal[0]=iStationVol[j];
				for(step=1;step<i&&j+step<iCardCount;)			//找一个连续i个值相等的数组(并保存于临时数组中)
				{
					if(iStationVol[j]==iStationVol[j+step])
					{
						CardTemp[step]=iCardList[j+step];			//用来保存牌形
						CardTempVal[step]=iStationVol[j+step];		//面值
						step++;
					}
					else
						break;
				}

			if(step>=i)			//找到一个连续i个相等的数组串起始位置为j,结束位置为j+setp-1
			{					//将从Start开始到j个数组后移setp个
				if(j!=Start)				//排除开始就是有序
				{
					for(;j>=Start;j--)					//从Start张至j张后移动i张
						{
							iCardList[j+i-1]=iCardList[j-1];
							iStationVol[j+i-1]=iStationVol[j-1];
						}
					for(int k=0;k<i;k++)				
					{
						iCardList[Start+k]=CardTemp[k];	//从Start开始设置成CardSave
						iStationVol[Start+k]=CardTempVal[k];
					}
				}
				Start=Start+i;
			}
			j=j+step-1;
		}
	}
	return true;
}

//获取扑克
BYTE CUpGradeGameLogic::GetCardFromHua(int iHuaKind, int iNum)
{
	if (iHuaKind!=UG_ERROR_HUA) return (iHuaKind+iNum-1);
	return 0;
}

//是否对牌
BOOL CUpGradeGameLogic::IsDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount<2)
		return FALSE;
	int temp[17]={0},itwo=0;
	for(int i=0;i<iCardCount;i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i=0;i<17;i++)
	{
		if(temp[i]==2)
			itwo++;
	}
	return (itwo==1);
}

//是否三条
BOOL CUpGradeGameLogic::IsThree(BYTE iCardList[], int iCardCount)
{
	if (iCardCount<3)
		return FALSE;
	int temp[17]={0},ithree=0;;
	for(int i=0;i<iCardCount;i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i=0;i<17;i++)
	{
		if(temp[i]==3)
			ithree++;
	}
	return (ithree==1);
}

//是否两对（两个对子）
BOOL CUpGradeGameLogic::IsCompleDouble(BYTE iCardList[], int iCardCount)
{
	if (iCardCount <4)
		return FALSE;

	int temp[17]={0},itwo=0;;
	for(int i=0;i<iCardCount;i++)
		temp[GetCardNum(iCardList[i])]++;

	for(int i=0;i<17;i++)
	{
		if(temp[i]==2)
			itwo++;
	}
	return (itwo==2);
}

//是否同花(同花为五张牌为一种花式)
BOOL CUpGradeGameLogic::IsSameHua(BYTE iCardList[], int iCardCount)
{
	//if (iCardCount != 5) return FALSE;
	//四个也可以算花
	if(iCardCount<4)
		return FALSE;
	int hs = -1;
	SortCard(iCardList,NULL,iCardCount);
	for (int i = 0;i < iCardCount;i++)
	{
		int hua = GetCardHuaKind(iCardList[i]);
		if (hs < 0)
		{
			hs = hua;
			continue;
		}
		if (hs != hua) return FALSE;
	}
	return TRUE;
}

//是否为葫芦（三张一样的牌带2张一样的牌）
BOOL CUpGradeGameLogic::IsHuLu(BYTE iCardList[], int iCardCount)
{
	if (iCardCount != 5) return FALSE;
	
	ReSortCard(iCardList,iCardCount);

	int pd0 = GetCardNum(iCardList[0]),
		pd1 = GetCardNum(iCardList[1]),
		pd2 = GetCardNum(iCardList[2]),
		pd3 = GetCardNum(iCardList[3]),
		pd4 = GetCardNum(iCardList[4]);

	return ((pd0 == pd1) && (pd1 == pd2) && (pd3 == pd4));
}

//是否为铁支（4张同样大小的牌）
BOOL CUpGradeGameLogic::IsTieZhi(BYTE iCardList[], int iCardCount)
{
	if (iCardCount < 4) 
		return FALSE;
	//int pd = -1;
	int temp[18]={0};
	for(int i=0;i<iCardCount;i++)
	{
		temp[GetCardNum(iCardList[i])]++;
	}

	for(int i=0;i<18;i++)
	{
		if(temp[i]==4)
			return  true;
	}
	return false;
	/*
	for (int i = 0;i < iCardCount;i++)
	{
		int dian = GetCardNum(iCardList[i]);
		if (pd < 0)
		{
			pd =dian;
			continue;
		}
		if (pd != dian) 
			return FALSE;
	}
	return TRUE;*/
}

//是否同花顺（5张）
BOOL CUpGradeGameLogic::IsSameHuaContinue(BYTE iCardList[], int iCardCount)
{
	//if (iCardCount != 5) 
	//	return FALSE;
	if(iCardCount<4)
		return FALSE;
	if (!IsSameHua(iCardList,iCardCount)) 
		return FALSE;
	if (!IsSingleContinue(iCardList,iCardCount)) return FALSE;

	return TRUE;
}

//最小顺子(a,8,9,10,j)
BOOL CUpGradeGameLogic::IsSmallSingleContinue(BYTE iCardList[],int iCardCount)
{
	if(iCardCount<4)
		return FALSE;
	BYTE Temp[18]={0};
	for(int i = 0; i < iCardCount; i ++)
	{
		Temp[GetCardNum(iCardList[i])] ++;
	}
	
	if(Temp[14] && Temp[10]&&Temp[9]&&Temp[8]&&Temp[11])
		return TRUE;
	
	return FALSE;
}

//同花最小顺子
BOOL CUpGradeGameLogic::IsSmallSameHuaContinue(BYTE iCardList[], int iCardCount)
{
	if (IsSameHua(iCardList, iCardCount))
	{
		if (IsSmallSingleContinue(iCardList, iCardCount))
			return TRUE;
	}

	return FALSE;
}

//是否单顺(至少5张)
BOOL CUpGradeGameLogic::IsSingleContinue(BYTE iCardList[], int iCardCount)
{
	//if (iCardCount != 5) return FALSE;
	if(iCardCount<4)
		return FALSE;
	
	//if (IsSameHua(iCardList,iCardCount)) return FALSE;

	SortCard(iCardList,NULL,iCardCount);
	
	int hs = -1;
	for (int i = 0;i < iCardCount - 1;i++)
	{
		//if (GetCardNum(iCardList[i]) == 2 || iCardList[i] == 0x4E || iCardList[i] == 0x4F)//不能含2和大小王
		//	return FALSE;
		if ((GetCardNum(iCardList[i]) % 15) != ((GetCardNum(iCardList[i+1]) + 1) % 15))
			return FALSE;
		
	}
	return TRUE;
}


//得到牌列花色
BYTE CUpGradeGameLogic::GetCardListHua(BYTE iCardList[], int iCardCount)
{
	int iHuaKind=GetCardHuaKind(iCardList[0]);
	if (GetCardHuaKind(iCardList[iCardCount-1])!=iHuaKind) return UG_ERROR_HUA;
	return iHuaKind;
}

//获取牌型
BYTE CUpGradeGameLogic::GetCardShape(BYTE iCardList[], int iCardCount)
{
	/***************************************************
	同花顺>铁支>葫芦>同花>顺子>三条>两对>对子>散牌
	***************************************************/

	if (iCardCount <= 0) 	return UG_ERROR_KIND;//非法牌
	if (IsSameHuaContinue(iCardList,iCardCount)) return SH_SAME_HUA_CONTINUE;//同花顺
	if (IsSmallSameHuaContinue(iCardList,iCardCount)&&(m_dwCardShape&(0x00000001<<1))) return SH_SMALL_SAME_HUA_CONTINUE;//同花最小顺子
	if (IsTieZhi(iCardList,iCardCount)) return SH_TIE_ZHI;//铁支
	if (IsHuLu(iCardList,iCardCount)) return SH_HU_LU;//葫芦
	if (IsSameHua(iCardList,iCardCount)) return SH_SAME_HUA;//同花
	if (IsSingleContinue(iCardList,iCardCount)) return SH_CONTINUE;//顺子
	if (IsSmallSingleContinue(iCardList,iCardCount)&&(m_dwCardShape&(0x00000001<<1))) return SH_SMALL_CONTINUE;	//最小顺子
	if (IsThree(iCardList,iCardCount)) return SH_THREE;//三条
	if (IsCompleDouble(iCardList,iCardCount)) return SH_TWO_DOUBLE;//两对
	if (IsDouble(iCardList,iCardCount)) return SH_DOUBLE;//对子

	return SH_OTHER;//散牌
}

//比较两手中牌的大小
int CUpGradeGameLogic::CompareCard(BYTE iFirstCard[], int iFirstCount, BYTE iSecondCard[], int iSecondCount)
{
	//return 1:iFirstCard > iSecondCard
	/***************************************************
	同花顺>铁支>葫芦>同花>顺子>三条>两对>对子>散牌
	***************************************************/
	BYTE iFirCard[5],iSecCard[5];

	::CopyMemory(iFirCard,iFirstCard,sizeof(BYTE)*iFirstCount);
	::CopyMemory(iSecCard,iSecondCard,sizeof(BYTE)*iSecondCount);


	BYTE iFirstCardKind = GetCardShape(iFirCard,iFirstCount),
		iSecondCardKind = GetCardShape(iSecCard,iSecondCount);

	ReSortCard(iFirCard,iFirstCount);
	ReSortCard(iSecCard,iSecondCount);

	//类型不同
	if (iFirstCardKind != iSecondCardKind) 
	{
		return (iFirstCardKind - iSecondCardKind > 0) ? 1 : -1;
	}

	//类型相同(先比较最大牌，后比较花色)
	switch (iFirstCardKind)
	{
	case SH_DOUBLE://对子
		{
			//对子大小
			int pd1 = GetCardNum(iFirCard[0]);
			int	pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			int hs1,hs2;
			switch(iFirstCount)//当前对子相等，比较其他牌
			{
			case 2://仅为单对比较大小
				{
					hs1 = GetCardHuaKind(iFirCard[0]);
					hs2 = GetCardHuaKind(iSecCard[0]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 3://仅只有一单张
				{
					pd1 = GetCardNum(iFirCard[2]);
					pd2 = GetCardNum(iSecCard[2]);
					if (pd1 != pd2)
				 {
					 return (pd1 - pd2 > 0 ? 1 : -1);
				 }
					hs1 = GetCardHuaKind(iFirCard[2]);
					hs2 = GetCardHuaKind(iSecCard[2]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 4://二单张
				{
					pd1 = GetCardNum(iFirCard[2]);
					pd2 = GetCardNum(iSecCard[2]);
					if (pd1 != pd2)
				 {
					 return (pd1 - pd2 > 0 ? 1 : -1);
				 }

					pd1 = GetCardNum(iFirCard[3]);
					pd2 = GetCardNum(iSecCard[3]);
					if (pd1 != pd2)
				 {
					 return (pd1 - pd2 > 0 ? 1 : -1);
				 }
					hs1 = GetCardHuaKind(iFirCard[3]);
					hs2 = GetCardHuaKind(iSecCard[3]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 5://三单张
				{
					pd1 = GetCardNum(iFirCard[2]);
					pd2 = GetCardNum(iSecCard[2]);
					if (pd1 != pd2)
				 {
					 return (pd1 - pd2 > 0 ? 1 : -1);
				 }

					pd1 = GetCardNum(iFirCard[3]);
					pd2 = GetCardNum(iSecCard[3]);
					if (pd1 != pd2)
				 {
					 return (pd1 - pd2 > 0 ? 1 : -1);
				 }

					pd1 = GetCardNum(iFirCard[4]);
					pd2 = GetCardNum(iSecCard[4]);
					if (pd1 != pd2)
				 {
					 return (pd1 - pd2 > 0 ? 1 : -1);
				 }
					//比较最大单牌花色
					hs1 = GetCardHuaKind(iFirCard[2]);
					hs2 = GetCardHuaKind(iSecCard[2]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			}
		}
	case SH_SAME_HUA_CONTINUE://同花顺
	case SH_SMALL_SAME_HUA_CONTINUE://最小同花顺
	case SH_SAME_HUA://同花
	case SH_CONTINUE://顺子
		{
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}

			int hs1 = GetCardHuaKind(iFirCard[0]),
				hs2 = GetCardHuaKind(iSecCard[0]);
			return (hs1 - hs2 > 0 ? 1 : -1);
		}

	case SH_TIE_ZHI://铁支
	case SH_HU_LU://葫芦
	case SH_THREE://三条
		{
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);

			return (pd1 - pd2 > 0 ? 1 : -1);

		}

	case SH_TWO_DOUBLE://对子先比较第一对,再比较第二对
		{
			//第一对比较
			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			//第二对比较
			pd1 = GetCardNum(iFirCard[2]);
			pd2 = GetCardNum(iSecCard[2]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			int hs1,hs2;

			switch(iFirstCount)
			{
			case 4://仅为二对，比较小对花色
				{
					hs1 = GetCardHuaKind(iFirCard[2]);
					hs2 = GetCardHuaKind(iSecCard[2]);	
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 5://仅一单张
				{
					pd1 = GetCardNum(iFirCard[4]);
					pd2 = GetCardNum(iSecCard[4]);
					if (pd1 != pd2)
				 {
					 return (pd1 - pd2 > 0 ? 1 : -1);
				 }
					hs1 = GetCardHuaKind(iFirCard[4]);
					hs2 = GetCardHuaKind(iSecCard[4]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			}
		}
	case SH_OTHER:	//单牌比较
		{
			SortCard(iFirCard,NULL,iFirstCount);
			SortCard(iSecCard,NULL,iSecondCount);

			int pd1 = GetCardNum(iFirCard[0]),
				pd2 = GetCardNum(iSecCard[0]);
			if (pd1 != pd2)
			{
				return (pd1 - pd2 > 0 ? 1 : -1);
			}
			int hs1,hs2;
			switch(iFirstCount)
			{
			case 1:
				{
					hs1 = GetCardHuaKind(iFirCard[0]);
					hs2 = GetCardHuaKind(iSecCard[0]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 2:
				{
					pd1 = GetCardNum(iFirCard[0]);
					pd2 = GetCardNum(iSecCard[0]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[1]);
					pd2 = GetCardNum(iSecCard[1]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					hs1 = GetCardHuaKind(iFirCard[1]);
					hs2 = GetCardHuaKind(iSecCard[1]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 3:
				{
					pd1 = GetCardNum(iFirCard[0]);
					pd2 = GetCardNum(iSecCard[0]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[1]);
					pd2 = GetCardNum(iSecCard[1]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[2]);
					pd2 = GetCardNum(iSecCard[2]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					hs1 = GetCardHuaKind(iFirCard[2]);
					hs2 = GetCardHuaKind(iSecCard[2]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 4:
				{
					pd1 = GetCardNum(iFirCard[0]);
					pd2 = GetCardNum(iSecCard[0]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[1]);
					pd2 = GetCardNum(iSecCard[1]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[2]);
					pd2 = GetCardNum(iSecCard[2]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[3]);
					pd2 = GetCardNum(iSecCard[3]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					hs1 = GetCardHuaKind(iFirCard[3]);
					hs2 = GetCardHuaKind(iSecCard[3]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}
			case 5:
				{
					pd1 = GetCardNum(iFirCard[0]);
					pd2 = GetCardNum(iSecCard[0]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[1]);
					pd2 = GetCardNum(iSecCard[1]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[2]);
					pd2 = GetCardNum(iSecCard[2]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[3]);
					pd2 = GetCardNum(iSecCard[3]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					pd1 = GetCardNum(iFirCard[4]);
					pd2 = GetCardNum(iSecCard[4]);
					if (pd1 != pd2)
					{
						return (pd1 - pd2 > 0 ? 1 : -1);
					}
					hs1 = GetCardHuaKind(iFirCard[0]);
					hs2 = GetCardHuaKind(iSecCard[0]);
					return (hs1 - hs2 > 0 ? 1 : -1);
				}	
			}
		}
	}
	return -1;
}


//对比单牌
BOOL CUpGradeGameLogic::CompareOnlyOne(BYTE iFirstCard, BYTE iNextCard)
{
	int iFirstNum = GetCardNum(iFirstCard);//上手牌
	int iNextNum  = GetCardNum(iNextCard);//本家牌

	if (iFirstCard == 0x4F) return FALSE;//大王
	if (iNextCard == 0x4F) return TRUE;

	if (iFirstCard == 0x4E) return FALSE;//小王
	if (iNextCard == 0x4E) return TRUE;

	if (iFirstNum == 2) return FALSE;//2
	if (iNextNum == 2) return TRUE;

	return ((iNextNum - iFirstNum) > 0 ? TRUE : FALSE);//其他

	/*int iFristHua=GetCardHuaKind(iFirstCard,FALSE);
	int iNextHua=GetCardHuaKind(iNextCard,FALSE);
	if (iFristHua!=iNextHua)
	{
		//不同花色对比
		if (iFristHua==UG_NT_CARD) return TRUE;
		return (iNextHua!=UG_NT_CARD);
	}
	
	//同花色对比
	return GetCardBulk(iFirstCard,FALSE)>=GetCardBulk(iNextCard,FALSE);*/
}

//自动出牌函数
BOOL CUpGradeGameLogic::AutoOutCard(BYTE iHandCard[], int iHandCardCount, BYTE iBaseCard[], int iBaseCardCount,
									BYTE iResultCard[], int & iResultCardCount, BOOL bFirstOut)
{
	if (bFirstOut == TRUE)
	{
		iResultCard[0] = iHandCard[iHandCardCount-1];
		iResultCardCount = 1;
	}
	return TRUE;
}


//清除 0 位扑克
int CUpGradeGameLogic::RemoveNummCard(BYTE iCardList[], int iCardCount)
{
	int iRemoveCount=0;
	for (int i=0;i<iCardCount;i++)
	{
		if (iCardList[i]!=0) iCardList[i-iRemoveCount]=iCardList[i];
		else iRemoveCount++;
	}
	return iRemoveCount;
}

//混乱扑克
BYTE CUpGradeGameLogic::RandCard(BYTE iCard[], int iCardCount)
{ 
	static const BYTE m_CardArray[54]={
		/*     2     3     4     5     6     7     8     9    10     J     Q     K     A*/
			0x01, 0x02 ,0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,		//方块 2 - A
			0x11, 0x12 ,0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,		//梅花 2 - A
			0x21, 0x22 ,0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,		//红桃 2 - A
			0x31, 0x32 ,0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,		//黑桃 2 - A
			0x4E, 0x4F};
			//小鬼，大鬼
 

	BYTE iSend=0,iStation=0,iCardList[162];
	srand((unsigned)time(NULL));
	
	
	int j = 0,n = 0;
	if(iCardCount!=28)
	  for (int i = 0;i < iCardCount;i += 12)
	    {
		  j = n * 13 + 1;
		  ::CopyMemory(&iCardList[i],&m_CardArray[j],sizeof(BYTE)*12);
		   n++;
	    }
	else //各门花色只取8-A
	{
		for (int i = 0;i < iCardCount;i += 7)
	    {
		  j = n * 13 + 6;
		  ::CopyMemory(&iCardList[i],&m_CardArray[j],sizeof(BYTE)*7);
		   n++;
	    }
	}
	//for (int i=0;i<iCardCount;i+=54)
	//	::CopyMemory(&iCardList[i],m_CardArray,sizeof(m_CardArray));

	do
	{
		iStation=rand()%(iCardCount-iSend);
		iCard[iSend]=iCardList[iStation];
		iSend++;
		iCardList[iStation]=iCardList[iCardCount-iSend];
	} while (iSend<iCardCount);

	//for (int i=0;i<108;i++)
	//	::CopyMemory(&iCard[i],&m_CardArray[53],1);

	return iCardCount;
}

//删除扑克
int CUpGradeGameLogic::RemoveCard(BYTE iRemoveCard[], int iRemoveCount, BYTE iCardList[], int iCardCount)
{
	//检验数据
	if ((iRemoveCount>iCardCount)) return 0;

	//把要删除的牌置零
	int iDeleteCount=0;
	for (int i=0;i<iRemoveCount;i++)
	{
		for (int j=0;j<iCardCount;j++)
		{
			if (iRemoveCard[i]==iCardList[j])
			{
				iDeleteCount++;
				iCardList[j]=0;
				break;
			}
		}
	}
	RemoveNummCard(iCardList,iCardCount);
	if (iDeleteCount!=iRemoveCount) return 0;

	return iDeleteCount;
}


//查找==iCard的单牌所在iCardList中的序号
int  CUpGradeGameLogic::GetSerialBySpecifyCard(BYTE iCardList[],int iStart,int iCardCount,BYTE iCard)
{
	for(int i = iStart;i < iCardCount;i ++)
	{
		if(iCardList[i] == iCard)
			return i;
	}
	return -1;
}