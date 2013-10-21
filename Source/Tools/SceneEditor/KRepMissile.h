// KRepMissile.h: interface for the KRepMissile class.
//
//////////////////////////////////////////////////////////////////////
          //                                                                                                    
          //                                                                                                    
          //                                                                                                    
          //                                                                              : .                   
          //                                                                                $                   
          //                                                                              B .                   
          //                                                                           .M                       
          //                                                                        . N.                        
          //                                              =xBBox                    M,                          
          //                                            "WWWNWWB.                 M.            ...             
          //                                            MMMMMMWN                W=           ......             
          //                                            MMMMMMMB ..,...     . Ro..         ......               
          //                                            MMMMMMMW.:====:,.   B9 ,         ...,...                
          //                                            ,MMMMMM::++t++"=. M$, .       ....,...                  
          //                                             +MMMMW""ttvvt":WR .        ...,,,..                    
          //                                             .MMMMWotvvvt+RW  .       ..,,,,,.                      
          //                                        ,  BMMMMMMMMMMMMMWt=,.      ..,,,,,.                        
          //                                     .,vWMMMMMMMMMMMMMMMMMM=.     ..,,,,,..                         
          //                                      W"WMMMMMMMMMMMMMMMMMM$.,..,,,,,,,..                           
          //                                     +vWo:2MMMMMMMMMMMMMMMMM======::,,.                             
          //                                     =MM"+xWMMMMMMMMMMMMMMMMMt+++"=:,                               
          //                                     +oM2O:MMMMMMMMMMMMMMMMMM2vtt+":,                               
          //                                     oMMx:2MMMMMMMMMMMMMMMMMMMvvvt+=,.                              
          //                                    ,+MMoNMMMMMMMMMMMMMMMMMMMMxvvt+":,                              
          //                                    tovM,",M+vMMMMMMMMWRNMMMMOvvvvt+=,.                             
          //                                    v2M=""WMWMMMMMMMMN$x9MMMMMxxvvvt"=,.                            
          //                                   oWMWM99WMMMMMMMMMtvvvvMMMMNvvvvvt+=,.                            
          //                                . MWMMMNWMMMMMMMMMMMMvvvvvMMWvvvvvvt+=,.                            
          //                               "MMMM= .MWMMMMMMMMMMMMvvvvvMMMtvtvvvt+=,.                            
          //                          . ,MMMMM.   2MMMMMMMMMMMMMMMvvvvMMM+++tttt+",.                            
          //                         .,,WMMNM:    MMMMMMMMMMMMMMMMOvvvMMM"="+ttt+=,.                            
          //                         ,MMWW   :  .2MMMMMMMMMMMMMMMMMvvMMMM::="+++":,.                            
          //                        WMM..      . MMMMMMMMMMMMMMMMMWvoMNMW:::="+"=:.                             
          //                    ..WWB       ..,=OMMMMMMMMMMMMMMMMMMvvvMM9"::="""=:.                             
          //                  . $BM        ..,="9MMMMMMMMMMMMMMMMMM2vv$tt+"=""++=:.                             
          //               ..,+MM,        .,:=""MMMMMMMMMMMMMMMMMMMtxvvvt+""++++":..                            
          //                22M:        .,:====,MMMMMMMMMMMMMMMMMMM2vvvvtt++++++":.                             
          //             .,Rx,        .,::====:vMMMMMMMMMMMMMMMMMMMMvxvvvt+"++""=,.                             
          //             W :        ..,:===:,,.OMMMMMMMMMMMMMMMMMMMMvvvvvt+"==::,.                              
          //         . W +..      ..,:===:,,.  MMMMMMMMMMB9MMMMMMMMMWvxvvt+=:,..                                
          //         M v        ..,:===:,,.   +MMMMMMMMMMvtMMMMMMMMMMvvvvt+=,.                                  
          //       9,..        .,::==:,..     WMMMMMMMMMWvvWWMMMMMMMMvvvvt+=,.                                  
          //   ..t:. .       .,,:==:,,.       MMMMMMMMMMBvvxMMMMMMMMMvvvvtt":,.                                 
          //    9=         .,:::::,,.         $MMMMMMMMMRvvvMMMMMMMMMtvvvvt+=:.                                 
          // =9R.        ..,::::,,..          NMMMMMMMMMxvvvRWMMMMMMM$vvvvt+=:,                                 
          //v".        ..,,:::,,..            MMMMMMMMMMtvvvvMMMMMMMMMvvvvt+":,.                                
          //          .,,,:,,..               MMMMMMMMMBvvvvvtMMMMMMMWtvvvt+":,.                                
          //        .,,::,,.                  MMMMMMMMMtvvvvvvMMMMMMMW9vvvt+"=,.                                
          //      ..,,,,,..                   MMMMMMMM+tvvvvvvoMMMMMMMWvvvvt"=,.                                
          //      ..,,,..                     MMMMMMMB+vvvvvvt+MMMMMMMMvvvvt+":.                                
          //         .                       $WMMMMMM2tvvvvvvt+MMMMMMMMvvvvt+":,.                               
          //                                9WMMMMMMW"tvvvvvt+"MMMMMMMMtvvvtt+=,.                               
          //                               .NMMMMMMM++tvvvvt+":BMMMMMMMtvvvvt+=:.                               
          //                                MMMMMMMR=+tvvvvt+":=WMMMMMMtvvvvt+":.                               
          //                                NNMMMMM,++tvvvvt+=,MMMMMMMMtvvvvtt":.                               
          //                           "NMWWWWMMMMR="tvvvvt+".WWMMMMMM$tvvvvtt":.                               
          //                      .=ttMMMMMM=xWWMMo22$$2o222BMMMMMMMMM$$$$$xtt":.                               
          //                      .MMW ,M  MMMMMMMMMMMMMMMMMM  MMM" MMWMMMMWt+":.                               
          //              MMMMMMMMWMMMM   MM.   vM .  M2    M  MMM" M"   +MMMMMMMMMWWx                          
          //    .$RNWMMMMNMMMMMMMMMMMMMR MMM.    M Mo R  M  M  MMM" M" M  MMMMMMMMMMMMMMMWWWNNW                 
          //     99BNWWMMMMMMMMMMMMMMMMO MMM.    M Mx RMt2  M     " M= M  MMMMMMMMMMMMMMMMMWNBB                 
          //YangLin 2005.2.20                                                                                  .,           
///////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_KREPMISSILE_H__71727A03_59DA_4DBD_BC35_B41863510CB6__INCLUDED_)
#define AFX_KREPMISSILE_H__71727A03_59DA_4DBD_BC35_B41863510CB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataObject.h"

class KRepMissile : public KComponentObject  
{
private:

public:
	KRepMissile();
	virtual ~KRepMissile();
public:
	HRESULT Render();

};

#endif // !defined(AFX_KREPMISSILE_H__71727A03_59DA_4DBD_BC35_B41863510CB6__INCLUDED_)
