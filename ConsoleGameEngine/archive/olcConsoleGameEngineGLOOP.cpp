/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9
License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
https://www.github.com/onelonecoder
https://www.onelonecoder.com
https://www.youtube.com/javidx9
GNU GPLv3
https://github.com/OneLoneCoder/videos/blob/master/LICENSE
From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!
Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.
Author
~~~~~~
Twitter: @javidx9	http://twitter.com/javidx9
Blog:				http://www.onelonecoder.com
YouTube:			http://www.youtube.com/javidx9
Videos:
~~~~~~
Original:				https://youtu.be/cWc0hgYwZyc
Added mouse support:	https://youtu.be/tdqc9hZhHxM
Beginners Guide:		https://youtu.be/u5BhrA8ED0o
Shout Outs!
~~~~~~~~~~~
Thanks to cool people who helped with testing, bug-finding and fixing!
wowLinh, JavaJack59, idkwid, kingtatgi, Return Null, CPP Guy
Last Updated: 18/03/2018
Usage:
~~~~~~
This class is abstract, so you must inherit from it. Override the OnUserCreate() function
with all the stuff you need for your application (for thready reasons it's best to do
this in this function and not your class constructor). Override the OnUserUpdate(float fElapsedTime)
function with the good stuff, it gives you the elapsed time since the last call so you
can modify your stuff dynamically. Both functions should return true, unless you need
the application to close.
int main()
{
// Use olcConsoleGameEngine derived app
OneLoneCoder_Example game;
// Create a console with resolution 160x100 characters
// Each character occupies 8x8 pixels
game.ConstructConsole(160, 100, 8, 8);
// Start the engine!
game.Start();
return 0;
}
Input is also handled for you - interrogate the m_keys[] array with the virtual
keycode you want to know about. bPressed is set for the frame the key is pressed down
in, bHeld is set if the key is held down, bReleased is set for the frame the key
is released in. The same applies to mouse! m_mousePosX and Y can be used to get
the current cursor position, and m_mouse[1..5] returns the mouse buttons.
The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.
There may be bugs!
See my other videos for examples!
http://www.youtube.com/javidx9
Lots of programs to try:
http://www.github.com/OneLoneCoder/videos
Chat on the Discord server:
https://discord.gg/WhwHUMV
Be bored by Twitch:
http://www.twitch.tv/javidx9
*/

#include "olcConsoleGameEngineGLOOP.h"

wglSwapInterval_t *wglSwapInterval;

// Based on the PxPlus IBM CGA Font from 
// "The Ultimate Oldschool PC Font Pack" http://int10h.org/oldschool-pc-fonts/
// Rendered into an image (256x56) and converted to char array
//
// The Ultimate Oldschool PC Font Pack is licensed under a Creative Commons
// Attribution-ShareAlike 4.0 International License.
//
// You should have received a copy of the license along with this work. If
//  not, see < http://creativecommons.org/licenses/by-sa/4.0/ >.
//
//                                                           (c) 2016 VileR
//
// (256x256x8)
unsigned char pxplus_ibm_cga[65536];
char pxplus_ibm_cga_enc[8509 + 1] =
"0mWOkP0780et0q?qqedtH40@HHVtq106HP10000000001stOLm320q??33cI<Kqk707?Vbbf0e3?HP130@26t3@YKtW?Le16WKFVeK6tshh7qnWIKh10sn76eHe0Vfh"
"t04mttrlOsehe2j[dVfPfWtlOHHVlk006HPfOt=etsn70r>a?sqlOq<a@r>3?<HqnOe76VQb=sn76s1S13Hktq0@VWh17[n36WKFVhPQ3Vhh1en70HhQOqP1?HeeOTq"
"?604mt8P0280etq<q<sreIK70@qH6khm76HP1000000000sn700e170q?0tp167<06000600P70q?0000000000003kH330eQ1HH00000000V?<mQ7mq37tmQ700060"
"HP70mQ=kmhHkH03<H630000e<V3h<3?3He<h<33<e00<<30NHhO3<373H06qe000006hf0<eHh730h<he036q36e0030HS7Hm606Pftt0e?0eeN<e17h0h7HmQ?00e0"
"00360e00t133p0P1He3300006q6360hOe<33h030<H00He0000P=OHk<0e03Vf030ee0Wfe8h0h<hee<He03<q3300030H33Sn60HH000e00<4P?tqS7mnQ7<mQ3<H0"
"60H03000000000000000060000000000000000000000000P?<q3?OqlOq<S7mMk3S=67tmf?Nqh<h<kHhqW73m120<V7VIV=6ITIhe0<VIeMWISIhHk<b<h<S=k<SI"
"P1He10p=SI3HV5F<e<<0S=6qlKSIk<VI03h<hHk<C<6e06k0eNhmh0VnQ73q33emQ1tblHs<S?<ee<ha67NPQ1HPfH0al?V=PIFHfLhee<kHiJh=V1pH36<<h<tf13<"
"I0<H0003<SIVISA6Hk<<<SIV=kHkHP7V=33hmfMkePI606600P7hq3?Oql3q=S7NLkOS=67?PhINmf?<<kHNqW70n10000000000000000000000000000000000000"
"000000et<0e10P30L0e1<0h1>0000000000200000000mPf1c1030H00e0P=0H000H03000000000e0000000003Hee>8PQ76m1<NHPKkm0<Vfe<Omf>cbS?s<h<S=k"
"<te06<00700S?hmh<?<SK<0S=<ql<hHk<c=03h<hJk<C6700>0H30sHk0hqS1hH63em13t=h<V=SINee<hq77he03He00S1e<V=h<3HP?Vfe<keeJh<S?sH0<a<S7tI"
"S?Ve06<0eH0mk>NmV7?0hIN<hIN<k<NH0<?q16cfP=S1h?mPf10q7000000000O000N0000000?P7000000000O00000000006He10hP1OSn3?0000sq?7<mP3H0Pt0"
"003L0ee3O030P16k<k<HH<01I3h00@@0H33HP13Vb=00mP=h<kH<3000mW9sm16L0@FkH600i40kq33<0PIK300<HSIh<SI<0063qeHt0P=0D4OhqSOE107<H060HVl"
"0003LeqkpfS10Pf06<63HH3051PIe0@C0003Nm00VQ=60m10Vf>hO?00HmlIsq36L0@Fsf<<0D50000000P?H3060mh<kK6ph006HqhH<Pf<044000001100t0000H0"
"k003000eqhHd7000600030m10s00000P?000000003000L00000<q33e0Pnf?Sf0ONL0>N<h1mmf<Oqh0PnhO33P?7PS7hPh3Ne17h007<Hh<00e<0000h0P=0P13S1"
"06S=600<300He<kH30NH30h<e?tqh?Nm10NHk<qm17LeS=h=h<0<h<s<hHSnf<SnfOhHP16H03<m13dMSIhHS=VffNh<h<h<SIKqlOhqhOh<S7NmQ7Ne03<ePItHk<S"
"=VIkqk<h<h<Nmh<S=k?h<k?hPQ16HP1<e03<Hh>qmQ=kehHW=h<h<336<kHS=h<S=hLeqh?tqS7NmQ7O<36<e17H0P?NmQ7Nmf3h000000000P70000000000000000"
"00000000000000007PSOt<3300e1mml<7eQ?hP1000P700030000N00070000<<00e00000033000<60qrf1m<h?he0@7Ph<hPS1hmQ7qmQ7Nmd7Nm1?NmP3Lm0<000"
"000000m3000000m30e03He03<e<e<hHk<<e06<mh7NmQ7Nmf?h=h<h<h<V=S?seW?smSt3qh?sr33<P13h<h<h<h<h0eJh<h<h<SIh<h<V=h<h<S73<P13e03Hee<h<"
"h<h<h<<Lk<h<h<smS?sn7tsnWOs3S7NeS7Nm1?Nmf<NmQ7Nm13smWOsn7<603000000000L000000000000000000000@000000mf3OqS?h<30007>sf16<HV=hal7e"
"qh?h<33<q70kHSOLHVI00P7Nm10q0eeke30qeQ7cI3O00P7N0006107L<a=qeS7N0P7hmQINe30Vn1?0q3HV1h?NqS7tmQ5NqS7q10O0<3<N0h?e<e<Vfh0hH6?VflK"
"sHe<6<S1hmf<6<S1cIPKtmh<s<S?3<e06<e03HPIVIk<NqS7tmf?FqS7t<l<h=h<hqh<h<SIhHV1V=SI6HVIk<S13He06<PA3He0VnSIs<SOhm73sfS7qe3?Ne3?ter"
"7crS7tmf?NqW?tm1Oee7<00000P1m000000000000000000000000e07000e70q16He3LNLWI7qh7tqf<hm13<0eqSQ7NWM00?pq3>q=k?e30VQf<Ntd100000mQ7<0"
"000<60<haI60PIaI03kMW1Vnk0c1P1VrP7>mQ30003>mQ3S=6<0HSIWI036eP16Hk0h<l<hHSOke03<eP7>e03<eeHS13<NHS=6ePA<HP16<l<V=h?cIVK<e03<e03<"
"e03<qkHh0S=NmQA<H636IPAVn3Os<SIVI63<e03<e03<e03drk<hHV=kH63tnQI6H6Oe03<hLVIWnQ7NmQ7NmQ7Nmf>dnf<WMVItn16Hql3t1e7Lq10000000000000"
"mP30000>0m1>m0000mP30000>qP3eQhH0Hh<6<60t1eHhe60q10Ler30kHSlme17N0036e000L60Lmf0W100tmS70eV=0000V107L<30kHh<0eV3qLk7drfI00eKOe1"
"0L0070<SOtaSIpqh>6mS?0HP?<Le3d=hNhqk7Oal<kmQ=NHS7hbQIcnSKVn633LP7<<036feNh<l<p=h<h=hHh<k<S=h<pnSIkHP?VQQ7NL06NePI<<l<S=hLh<hHhH"
"h<k<S=hHh6kHeI?HS1h03>m<3<NqW7S=36<<k<h<k<Lm17NeQ7qnlI?P16WrP7OqQ7Nq1000000me10000mP10000000000000>m00000000e17<S=tPe<hq32t0e?0"
"<h<<e0K0<30LeQ7N<3eme03k<37NeA;<mQ7be00t0e?Nm10<00Kh0P=k<h<0q70t1eONHh<seP?te03s<30h0000<30h0e<h00000e<hqh<t<h?6He0<e@;sq33h<h<"
"h<h<h<h<h<h<[=k<h<C6ITA6ITQ1Hm13<e03<mf<h<h<h<h<h<h<h<hO[nf<Ne43<f0C<He<em1;<e23<=h<h<h<h<h<h<h<hLlO<m33VIRIVHV96mf7HPQ7Hm1>NmW"
"7snQONmW7snQ?SIS7emfOtql?trh300e3>0000000000000000000e17000P700000000000L0q3?q07LeQS70q320q32KmhH<e10cf60H0A60iGKrP=0he0300P?0="
"30be00<eQ30m30k0e>0e033l30=jF6Hm760m1Os?W?6m33smV7k00000P=0000<@00VQ13aQ1?KeQ7eH3<p=733e03p<hH00000e10000000PIHe3OHPf6e<S?tntK["
"QQ7<ee4h000000000000e000m70<e66Haf<t<h<hLkIh033aaS7000000000000<000V10CaQf6>mf<s=WtsmS7Om16cf000000000600P100PI0e7Kqm0000000000"
"1e03<e00t00000000P30000000000000MiF7@ee?tPeOt=37NL62S=kOLqh?trh<8<kHL<h<HeP=bIheNHk<LHiHhH33VffMW=V=hHVIb<S?SbV=00000<6KSQf<V=P"
"=F4h<SfP=kqlK0<k<Vf03haV=[=V7hmV?SQhHH<S?3<V7HqhO<mfHtbW?S=S?HeP7[fQ?Sfe<p<eH@<66tHk0SI1Ch<63k<kJh1eHhH03<eP?LPeH<mf<>H34kPf<V="
"eH6Ik<kePIS=kHSIh<6H63<PP=8H33<ah0LPhMe<h?3qlOt=37NLkHS=kOL<h3tnQ78<62lnQ7cn3000000000000000000000000000000000000000000e03@0P70"
"m10t0P700P300e?00000000000000e<he0300ee0<30h0@60<300a100e00000000000000000000@7<0PKh<S1se@7eee<e<h<6mf?NmS?t<h>hakH<<S7hL33Vbh7"
"Nmf0<LS7<a1?h<3?h<h<Sb13haV7[b63h<h<kePIh<33hm06k<33?Hh<hPf<h<h0hee<[feJ[fe<h<S=<Hk>Omf<30S=hee6hqW7e<h<Omh<<<S?NmhJ<<h<hH36qmk"
"0hmQ?hHS7H<h<3eP=N<h0Pn16NPe<8H36NmQ700000<P700P7000000e00e100<0>00008002000000060ef1kq1LqmQ7hPl7Kq167ef<<ql?tflO[nfHLL6NS1P7k0"
"0060PIhe00eXf66L600<S76IVIVIiJh<kHVfkMS=hHtrW?t=P1<m1<oaQ?k<kHh<S1VITIFa6<h=W=VrlJOqWA6IVIOe03<0SFtHV7h=k<hmS?6HV7sefNpnQItbk<S"
"nQ7VIi0He03hXjFVIhNs=h?VIV1VIfJeqkKkHkJ[rQ=6ITI6Hk<<ee<JbUIVr6Hh<SIVIPI6bk<WMVIV=V=3efOtMh3qmQ7Nmf>pLkIWnh?hqh??qlO[nfHSMkHS1e0"
"000000000000000000000e000000S10000000000hef?teh?hPeHh<hJ[MeH3qf<s10000000000000000e<k<SIVjg<s<k<hakJ6<k0eak<000000000000L0000<h"
"HhHk0<<hJk<h<[bV1S=0H[=S7trhOqnfJs<kHWQlHt<k<s<03sa67hmhJ[nhKOelKs0S1VIVIhakHh=W=aMl<S=S13e0<[ff<eakJVbj<PbV=smS?6Hk?sPhNpnQIt="
"S=hHPI<<hJk<3<[bVIK=3<[=h<VIV1V=eJSrkKkHkJhef<?eS7NmhHt1hOtnhKOqf<hnk?tqeONaV?WMkIS=6000000000800H000H0000000000000<60000000000"
"0000000000000000000000000e1kH000003h0300H0L000000000P000000000000000000?07000000000?0e<Nqh>Nqh<s<k<hakJ7<k0s<SONmQ10mS?>m0<OaQ1"
"W=h<hHk<b<hJk<h<[bV1S=eH[=h<hHhOS=03<0S6KHS=t<h<V=03ha67hmhJ[nhKOPlKsqh?cIk3Ne03eXh?cnf<h<S?heP?sHh<eakJVbj<SbV=3<PI6<6<<ee<JbU"
"Ik<S7hHP7N032Sr7<trW?drQ?h<W7NLk3sqQ7N<h>pLkI0000?000Oe10P100P10000000000e0000000N0000000<00H0mlHOefOtm0?t<3?trh0ta7><qSItqhHSr"
"h?[M000q7H0H6<e03HHPfH[16HeqSIs166VIVIVIVIP1kJ0<h<6q70V13<e066H<kJP13<P=VIePfHVIVIke6HPbk<h<S160eJe03<SQ1<S=6<e03HS=6<H<VIcn67m"
"H6H[bh<hHP10<3<m0hHH0hHS10<e0hHS136SI6HPf0<k0kItmh<6H00S13?e<66H<k<003<H<kHePQ=k06H<0V5P=h=eqh3?0eHtI3<SQ13Sr10eq33tb7<Nef7Pr73"
"tI0HO0e7<000000000000000000<00000030000H0e006000t<h<hPQI00007L0Le=kHmL0000000000He00<HVI0<SIVIVIHHVKtq70000000030L0000000003<00"
"3hH60hHVIVI63hah<k<kHS=kHSQf<000000000eP10Pf<h0PIVIVIV100H<P=[bkJ[bkJs<h<tmS?tt?00000000000PIV16000063HhOtrlOtrl<N<300000000000"
"0000000HVIP1000Pf0kLlOlrlMt=33s0000000t300<0000H60WI6H0000H<P=SIhHk<V=Nm1<0000000000030000V100000000000000000000000q1000000q?00"
"H0000e<0e0300000P1K6000Vr?0s060Hm17se17<0P700000000<e0000e<<H33HHPI00eHe0076H0<kH330H30000000e?tP100aQ1KP13<H6000060mQ3NP17qqS7"
"k00000000e03q006<0000H06V1000e00sPQ=<H3<<0P=<mP3Hm17<e0?0PQ10000<ePI00006006>e13Le130HS3HP176H03tPfk0a=00006600000e000000000000"
"00e03<mQ3Ne030a=0I3000000V1eH04P?000000000000<H06sPQ=<e000000000000000m300<600000000000P7Nm06>e1000000q77O00?>CeqOh103?qe3?qe7h"
"qe??qf?t<0H00000006Ih<OIkH>L[sk0eHS=VISI6Kc=kHSfkHaf0?000000P5d<CD?<h6t[oHN<h<h<h<hHh?hLh<kLS=6mW?Le130mQ1OEU1Naf>:>k<KalNtbttq"
"htNqb7OqblOH0S=ke00FqeHEr0O><?0kq3tLh<hqeh?qef7LtrlLK06He1?tmQ1V=OAVnb5h0P=3HarVIVMkIWMsnWMkIWM<P13k0330qe?Sj7?Kle<0LW7S=qe3?qe"
"3?qkKdrkKd=0He07Le0060em000L0000000He17Le17Le17Le17Le1000P10HP17PQ07tr30P10q00P7e1000PQ1sm3000760P16H@2?qH3<LHeHh00<00300<3kc1h"
"?<eeHSQ00HS10H036VnWOemW=3<V10P10eml0hPr>t1P1H<kLL00kH006tQftHPQ?K?k7Sfe?<P1<K?e<H006te03[bW=tP160P1<sI66s=hkS=eH60P1HLhk3<36cf"
"00HHeHd=k0HPftHP1?Tm7?hmlH6<k<0<00kmlOhaf>tr300<kIS=06K0060P10qPQ7<q77Sr30100?00e<K0P10qh?smhO0Pf60P100006t3P100eH00000P3000P30"
"00000000000Hm00H000HP16H006H00K00000000He6KHe6KHe6KHe6K00000006HP160P160e6000000006af66af66af66af6000000P16HP10HPftaQ?0qt10tQ?K"
"at1KdQ?Kat1Kdr?0t300HP16H006H00KH003H00HHe63He6HHe63He6H0000mt1sOPt7ttttth6sqhs7tr6sqht7tr7sahs7drkttt>6H000HP160P10aQ1KaQ1Ka10"
"000000P1KaQ1KaQ1KaQ160006HP10H00KHe6KHe6K00000000He6KHe6KHe6KHP100P16H0060e66af66af6000000006af66af66af66af66afkt00et?0?00D5000"
"0000@0004200ett3POsQ1KaQ1Kar?00ip3e30AZ2000000HLetef1?qqte04mtthkpthkpt300tt0qT8BGqm30000?Oq?OkHVOWW97Uc=0a106a1et00er?0?00X>?k"
"eS?0mlOsrlH2nlerJCPttttttttd300tts3e300GfS=qHSOtt1?qIS@s=Ldkipe0000He6K00ettt0qT@TZqm3?smlt7P1LLHVOWW97IN>00006af60ttOd?0?0Ri50"
"0000s1@00042qeht33@Pt3000P1Ka1etttt3e300Z2000000000000000qtt0mWOHe3q8eQ=8edt0h7O33000000000000000000000000ekV1>2sq77<K<ekIFV000"
"0000000000000000000000eSIeh17tn3ts3V1Vr90000000000000000000000000WKVdsqlOtfPfPrlK?3000000000000000000000000enq<hOtnS?<HqHVIkn00"
"00000000000000000000000e36hmhJLeQ3VKSIVr?0000000000000000000000000Kpl<8P028qeIqqspW300000000000000000000000006Hm17L0007<0600et0";
void font_decode_custom_base64(void)
{
	int i, o = 0;
	for (i = 0; i < 8509; i++)
	{
		unsigned c = pxplus_ibm_cga_enc[i];
		c = c < 97 ? c - '0' : c + 44 - 'a';
		pxplus_ibm_cga[o++] = ((c >> 0) & 1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c >> 1) & 1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c >> 2) & 1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c >> 3) & 1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c >> 4) & 1) * 0xFF;
		pxplus_ibm_cga[o++] = ((c >> 5) & 1) * 0xFF;
	}
}

#define FUV(a,b,c) case a: *x=b; *y=c; break;

void GetFontCoords(int id, int *x, int *y) {
	switch (id) {
		FUV(0, 0, 0)FUV(1, 8, 0)FUV(2, 16, 0)FUV(3, 24, 0)FUV(4, 32, 0)FUV(5, 40, 0)FUV(6, 48, 0)
			FUV(7, 56, 0)FUV(8, 64, 0)FUV(9, 72, 0)FUV(10, 80, 0)FUV(11, 88, 0)FUV(12, 96, 0)FUV(13, 104, 0)
			FUV(14, 112, 0)FUV(15, 120, 0)FUV(16, 128, 0)FUV(17, 136, 0)FUV(18, 144, 0)FUV(19, 152, 0)FUV(20, 160, 0)
			FUV(21, 168, 0)FUV(22, 176, 0)FUV(23, 184, 0)FUV(24, 192, 0)FUV(25, 200, 0)FUV(26, 208, 0)FUV(27, 216, 0)
			FUV(28, 224, 0)FUV(29, 232, 0)FUV(30, 240, 0)FUV(31, 248, 0)FUV(32, 0, 8)FUV(33, 8, 8)FUV(34, 16, 8)
			FUV(35, 24, 8)FUV(36, 32, 8)FUV(37, 40, 8)FUV(38, 48, 8)FUV(39, 56, 8)FUV(40, 64, 8)FUV(41, 72, 8)
			FUV(42, 80, 8)FUV(43, 88, 8)FUV(44, 96, 8)FUV(45, 104, 8)FUV(46, 112, 8)FUV(47, 120, 8)FUV(48, 128, 8)
			FUV(49, 136, 8)FUV(50, 144, 8)FUV(51, 152, 8)FUV(52, 160, 8)FUV(53, 168, 8)FUV(54, 176, 8)FUV(55, 184, 8)
			FUV(56, 192, 8)FUV(57, 200, 8)FUV(58, 208, 8)FUV(59, 216, 8)FUV(60, 224, 8)FUV(61, 232, 8)FUV(62, 240, 8)
			FUV(63, 248, 8)FUV(64, 0, 16)FUV(65, 8, 16)FUV(66, 16, 16)FUV(67, 24, 16)FUV(68, 32, 16)FUV(69, 40, 16)
			FUV(70, 48, 16)FUV(71, 56, 16)FUV(72, 64, 16)FUV(73, 72, 16)FUV(74, 80, 16)FUV(75, 88, 16)FUV(76, 96, 16)
			FUV(77, 104, 16)FUV(78, 112, 16)FUV(79, 120, 16)FUV(80, 128, 16)FUV(81, 136, 16)FUV(82, 144, 16)FUV(83, 152, 16)
			FUV(84, 160, 16)FUV(85, 168, 16)FUV(86, 176, 16)FUV(87, 184, 16)FUV(88, 192, 16)FUV(89, 200, 16)FUV(90, 208, 16)
			FUV(91, 216, 16)FUV(92, 224, 16)FUV(93, 232, 16)FUV(94, 240, 16)FUV(95, 248, 16)FUV(96, 0, 24)FUV(97, 8, 24)
			FUV(98, 16, 24)FUV(99, 24, 24)FUV(100, 32, 24)FUV(101, 40, 24)FUV(102, 48, 24)FUV(103, 56, 24)FUV(104, 64, 24)
			FUV(105, 72, 24)FUV(106, 80, 24)FUV(107, 88, 24)FUV(108, 96, 24)FUV(109, 104, 24)FUV(110, 112, 24)FUV(111, 120, 24)
			FUV(112, 128, 24)FUV(113, 136, 24)FUV(114, 144, 24)FUV(115, 152, 24)FUV(116, 160, 24)FUV(117, 168, 24)FUV(118, 176, 24)
			FUV(119, 184, 24)FUV(120, 192, 24)FUV(121, 200, 24)FUV(122, 208, 24)FUV(123, 216, 24)FUV(124, 224, 24)FUV(125, 232, 24)
			FUV(126, 240, 24)FUV(127, 248, 24)FUV(160, 0, 32)FUV(161, 8, 32)FUV(162, 16, 32)FUV(163, 24, 32)FUV(164, 32, 32)
			FUV(165, 40, 32)FUV(166, 48, 32)FUV(167, 56, 32)FUV(168, 64, 32)FUV(169, 72, 32)FUV(170, 80, 32)FUV(171, 88, 32)
			FUV(172, 96, 32)FUV(173, 104, 32)FUV(174, 112, 32)FUV(175, 120, 32)FUV(176, 128, 32)FUV(177, 136, 32)FUV(178, 144, 32)
			FUV(179, 152, 32)FUV(180, 160, 32)FUV(181, 168, 32)FUV(182, 176, 32)FUV(183, 184, 32)FUV(184, 192, 32)FUV(185, 200, 32)
			FUV(186, 208, 32)FUV(187, 216, 32)FUV(188, 224, 32)FUV(189, 232, 32)FUV(190, 240, 32)FUV(191, 248, 32)FUV(192, 0, 40)
			FUV(193, 8, 40)FUV(194, 16, 40)FUV(195, 24, 40)FUV(196, 32, 40)FUV(197, 40, 40)FUV(198, 48, 40)FUV(199, 56, 40)
			FUV(200, 64, 40)FUV(201, 72, 40)FUV(202, 80, 40)FUV(203, 88, 40)FUV(204, 96, 40)FUV(205, 104, 40)FUV(206, 112, 40)
			FUV(207, 120, 40)FUV(208, 128, 40)FUV(209, 136, 40)FUV(210, 144, 40)FUV(211, 152, 40)FUV(212, 160, 40)FUV(213, 168, 40)
			FUV(214, 176, 40)FUV(215, 184, 40)FUV(216, 192, 40)FUV(217, 200, 40)FUV(218, 208, 40)FUV(219, 216, 40)FUV(220, 224, 40)
			FUV(221, 232, 40)FUV(222, 240, 40)FUV(223, 248, 40)FUV(224, 0, 48)FUV(225, 8, 48)FUV(226, 16, 48)FUV(227, 24, 48)
			FUV(228, 32, 48)FUV(229, 40, 48)FUV(230, 48, 48)FUV(231, 56, 48)FUV(232, 64, 48)FUV(233, 72, 48)FUV(234, 80, 48)
			FUV(235, 88, 48)FUV(236, 96, 48)FUV(237, 104, 48)FUV(238, 112, 48)FUV(239, 120, 48)FUV(240, 128, 48)FUV(241, 136, 48)
			FUV(242, 144, 48)FUV(243, 152, 48)FUV(244, 160, 48)FUV(245, 168, 48)FUV(246, 176, 48)FUV(247, 184, 48)FUV(248, 192, 48)
			FUV(249, 200, 48)FUV(250, 208, 48)FUV(251, 216, 48)FUV(252, 224, 48)FUV(253, 232, 48)FUV(254, 240, 48)FUV(255, 248, 48)
			FUV(256, 0, 56)FUV(257, 8, 56)FUV(258, 16, 56)FUV(259, 24, 56)FUV(260, 32, 56)FUV(261, 40, 56)FUV(262, 48, 56)
			FUV(263, 56, 56)FUV(264, 64, 56)FUV(265, 72, 56)FUV(266, 80, 56)FUV(267, 88, 56)FUV(268, 96, 56)FUV(269, 104, 56)
			FUV(270, 112, 56)FUV(271, 120, 56)FUV(272, 128, 56)FUV(273, 136, 56)FUV(274, 144, 56)FUV(275, 152, 56)FUV(276, 160, 56)
			FUV(277, 168, 56)FUV(278, 176, 56)FUV(279, 184, 56)FUV(280, 192, 56)FUV(281, 200, 56)FUV(282, 208, 56)FUV(283, 216, 56)
			FUV(284, 224, 56)FUV(285, 232, 56)FUV(286, 240, 56)FUV(287, 248, 56)FUV(288, 0, 64)FUV(289, 8, 64)FUV(290, 16, 64)
			FUV(291, 24, 64)FUV(292, 32, 64)FUV(293, 40, 64)FUV(294, 48, 64)FUV(295, 56, 64)FUV(296, 64, 64)FUV(297, 72, 64)
			FUV(298, 80, 64)FUV(299, 88, 64)FUV(300, 96, 64)FUV(301, 104, 64)FUV(302, 112, 64)FUV(303, 120, 64)FUV(304, 128, 64)
			FUV(305, 136, 64)FUV(306, 144, 64)FUV(307, 152, 64)FUV(308, 160, 64)FUV(309, 168, 64)FUV(310, 176, 64)FUV(311, 184, 64)
			FUV(312, 192, 64)FUV(313, 200, 64)FUV(314, 208, 64)FUV(315, 216, 64)FUV(316, 224, 64)FUV(317, 232, 64)FUV(318, 240, 64)
			FUV(319, 248, 64)FUV(320, 0, 72)FUV(321, 8, 72)FUV(322, 16, 72)FUV(323, 24, 72)FUV(324, 32, 72)FUV(325, 40, 72)
			FUV(326, 48, 72)FUV(327, 56, 72)FUV(328, 64, 72)FUV(329, 72, 72)FUV(330, 80, 72)FUV(331, 88, 72)FUV(332, 96, 72)
			FUV(333, 104, 72)FUV(334, 112, 72)FUV(335, 120, 72)FUV(336, 128, 72)FUV(337, 136, 72)FUV(338, 144, 72)FUV(339, 152, 72)
			FUV(340, 160, 72)FUV(341, 168, 72)FUV(342, 176, 72)FUV(343, 184, 72)FUV(344, 192, 72)FUV(345, 200, 72)FUV(346, 208, 72)
			FUV(347, 216, 72)FUV(348, 224, 72)FUV(349, 232, 72)FUV(350, 240, 72)FUV(351, 248, 72)FUV(352, 0, 80)FUV(353, 8, 80)
			FUV(354, 16, 80)FUV(355, 24, 80)FUV(356, 32, 80)FUV(357, 40, 80)FUV(358, 48, 80)FUV(359, 56, 80)FUV(360, 64, 80)
			FUV(361, 72, 80)FUV(362, 80, 80)FUV(363, 88, 80)FUV(364, 96, 80)FUV(365, 104, 80)FUV(366, 112, 80)FUV(367, 120, 80)
			FUV(368, 128, 80)FUV(369, 136, 80)FUV(370, 144, 80)FUV(371, 152, 80)FUV(372, 160, 80)FUV(373, 168, 80)FUV(374, 176, 80)
			FUV(375, 184, 80)FUV(376, 192, 80)FUV(377, 200, 80)FUV(378, 208, 80)FUV(379, 216, 80)FUV(380, 224, 80)FUV(381, 232, 80)
			FUV(382, 240, 80)FUV(383, 248, 80)FUV(402, 0, 88)FUV(417, 8, 88)FUV(439, 16, 88)FUV(506, 24, 88)FUV(507, 32, 88)
			FUV(508, 40, 88)FUV(509, 48, 88)FUV(510, 56, 88)FUV(511, 64, 88)FUV(536, 72, 88)FUV(537, 80, 88)FUV(538, 88, 88)
			FUV(539, 96, 88)FUV(593, 104, 88)FUV(632, 112, 88)FUV(710, 120, 88)FUV(711, 128, 88)FUV(713, 136, 88)FUV(728, 144, 88)
			FUV(729, 152, 88)FUV(730, 160, 88)FUV(731, 168, 88)FUV(732, 176, 88)FUV(733, 184, 88)FUV(894, 192, 88)FUV(900, 200, 88)
			FUV(901, 208, 88)FUV(902, 216, 88)FUV(903, 224, 88)FUV(904, 232, 88)FUV(905, 240, 88)FUV(906, 248, 88)FUV(908, 0, 96)
			FUV(910, 8, 96)FUV(911, 16, 96)FUV(912, 24, 96)FUV(913, 32, 96)FUV(914, 40, 96)FUV(915, 48, 96)FUV(916, 56, 96)
			FUV(917, 64, 96)FUV(918, 72, 96)FUV(919, 80, 96)FUV(920, 88, 96)FUV(921, 96, 96)FUV(922, 104, 96)FUV(923, 112, 96)
			FUV(924, 120, 96)FUV(925, 128, 96)FUV(926, 136, 96)FUV(927, 144, 96)FUV(928, 152, 96)FUV(929, 160, 96)FUV(931, 168, 96)
			FUV(932, 176, 96)FUV(933, 184, 96)FUV(934, 192, 96)FUV(935, 200, 96)FUV(936, 208, 96)FUV(937, 216, 96)FUV(938, 224, 96)
			FUV(939, 232, 96)FUV(940, 240, 96)FUV(941, 248, 96)FUV(942, 0, 104)FUV(943, 8, 104)FUV(944, 16, 104)FUV(945, 24, 104)
			FUV(946, 32, 104)FUV(947, 40, 104)FUV(948, 48, 104)FUV(949, 56, 104)FUV(950, 64, 104)FUV(951, 72, 104)FUV(952, 80, 104)
			FUV(953, 88, 104)FUV(954, 96, 104)FUV(955, 104, 104)FUV(956, 112, 104)FUV(957, 120, 104)FUV(958, 128, 104)FUV(959, 136, 104)
			FUV(960, 144, 104)FUV(961, 152, 104)FUV(962, 160, 104)FUV(963, 168, 104)FUV(964, 176, 104)FUV(965, 184, 104)FUV(966, 192, 104)
			FUV(967, 200, 104)FUV(968, 208, 104)FUV(969, 216, 104)FUV(970, 224, 104)FUV(971, 232, 104)FUV(972, 240, 104)FUV(973, 248, 104)
			FUV(974, 0, 112)FUV(976, 8, 112)FUV(1012, 16, 112)FUV(1024, 24, 112)FUV(1025, 32, 112)FUV(1026, 40, 112)FUV(1027, 48, 112)
			FUV(1028, 56, 112)FUV(1029, 64, 112)FUV(1030, 72, 112)FUV(1031, 80, 112)FUV(1032, 88, 112)FUV(1033, 96, 112)FUV(1034, 104, 112)
			FUV(1035, 112, 112)FUV(1036, 120, 112)FUV(1037, 128, 112)FUV(1038, 136, 112)FUV(1039, 144, 112)FUV(1040, 152, 112)FUV(1041, 160, 112)
			FUV(1042, 168, 112)FUV(1043, 176, 112)FUV(1044, 184, 112)FUV(1045, 192, 112)FUV(1046, 200, 112)FUV(1047, 208, 112)FUV(1048, 216, 112)
			FUV(1049, 224, 112)FUV(1050, 232, 112)FUV(1051, 240, 112)FUV(1052, 248, 112)FUV(1053, 0, 120)FUV(1054, 8, 120)FUV(1055, 16, 120)
			FUV(1056, 24, 120)FUV(1057, 32, 120)FUV(1058, 40, 120)FUV(1059, 48, 120)FUV(1060, 56, 120)FUV(1061, 64, 120)FUV(1062, 72, 120)
			FUV(1063, 80, 120)FUV(1064, 88, 120)FUV(1065, 96, 120)FUV(1066, 104, 120)FUV(1067, 112, 120)FUV(1068, 120, 120)FUV(1069, 128, 120)
			FUV(1070, 136, 120)FUV(1071, 144, 120)FUV(1072, 152, 120)FUV(1073, 160, 120)FUV(1074, 168, 120)FUV(1075, 176, 120)FUV(1076, 184, 120)
			FUV(1077, 192, 120)FUV(1078, 200, 120)FUV(1079, 208, 120)FUV(1080, 216, 120)FUV(1081, 224, 120)FUV(1082, 232, 120)FUV(1083, 240, 120)
			FUV(1084, 248, 120)FUV(1085, 0, 128)FUV(1086, 8, 128)FUV(1087, 16, 128)FUV(1088, 24, 128)FUV(1089, 32, 128)FUV(1090, 40, 128)
			FUV(1091, 48, 128)FUV(1092, 56, 128)FUV(1093, 64, 128)FUV(1094, 72, 128)FUV(1095, 80, 128)FUV(1096, 88, 128)FUV(1097, 96, 128)
			FUV(1098, 104, 128)FUV(1099, 112, 128)FUV(1100, 120, 128)FUV(1101, 128, 128)FUV(1102, 136, 128)FUV(1103, 144, 128)FUV(1104, 152, 128)
			FUV(1105, 160, 128)FUV(1106, 168, 128)FUV(1107, 176, 128)FUV(1108, 184, 128)FUV(1109, 192, 128)FUV(1110, 200, 128)FUV(1111, 208, 128)
			FUV(1112, 216, 128)FUV(1113, 224, 128)FUV(1114, 232, 128)FUV(1115, 240, 128)FUV(1116, 248, 128)FUV(1117, 0, 136)FUV(1118, 8, 136)
			FUV(1119, 16, 136)FUV(1168, 24, 136)FUV(1169, 32, 136)FUV(1470, 40, 136)FUV(1488, 48, 136)FUV(1489, 56, 136)FUV(1490, 64, 136)
			FUV(1491, 72, 136)FUV(1492, 80, 136)FUV(1493, 88, 136)FUV(1494, 96, 136)FUV(1495, 104, 136)FUV(1496, 112, 136)FUV(1497, 120, 136)
			FUV(1498, 128, 136)FUV(1499, 136, 136)FUV(1500, 144, 136)FUV(1501, 152, 136)FUV(1502, 160, 136)FUV(1503, 168, 136)FUV(1504, 176, 136)
			FUV(1505, 184, 136)FUV(1506, 192, 136)FUV(1507, 200, 136)FUV(1508, 208, 136)FUV(1509, 216, 136)FUV(1510, 224, 136)FUV(1511, 232, 136)
			FUV(1512, 240, 136)FUV(1513, 248, 136)FUV(1514, 0, 144)FUV(1520, 8, 144)FUV(1521, 16, 144)FUV(1522, 24, 144)FUV(1523, 32, 144)
			FUV(1524, 40, 144)FUV(7451, 48, 144)FUV(7462, 56, 144)FUV(7464, 64, 144)FUV(7808, 72, 144)FUV(7809, 80, 144)FUV(7810, 88, 144)
			FUV(7811, 96, 144)FUV(7812, 104, 144)FUV(7813, 112, 144)FUV(7839, 120, 144)FUV(7922, 128, 144)FUV(7923, 136, 144)FUV(8208, 144, 144)
			FUV(8210, 152, 144)FUV(8211, 160, 144)FUV(8212, 168, 144)FUV(8213, 176, 144)FUV(8215, 184, 144)FUV(8216, 192, 144)FUV(8217, 200, 144)
			FUV(8218, 208, 144)FUV(8219, 216, 144)FUV(8220, 224, 144)FUV(8221, 232, 144)FUV(8222, 240, 144)FUV(8223, 248, 144)FUV(8224, 0, 152)
			FUV(8225, 8, 152)FUV(8226, 16, 152)FUV(8230, 24, 152)FUV(8231, 32, 152)FUV(8240, 40, 152)FUV(8242, 48, 152)FUV(8243, 56, 152)
			FUV(8245, 64, 152)FUV(8249, 72, 152)FUV(8250, 80, 152)FUV(8252, 88, 152)FUV(8254, 96, 152)FUV(8255, 104, 152)FUV(8256, 112, 152)
			FUV(8260, 120, 152)FUV(8276, 128, 152)FUV(8308, 136, 152)FUV(8309, 144, 152)FUV(8310, 152, 152)FUV(8311, 160, 152)FUV(8312, 168, 152)
			FUV(8313, 176, 152)FUV(8314, 184, 152)FUV(8315, 192, 152)FUV(8319, 200, 152)FUV(8321, 208, 152)FUV(8322, 216, 152)FUV(8323, 224, 152)
			FUV(8324, 232, 152)FUV(8325, 240, 152)FUV(8326, 248, 152)FUV(8327, 0, 160)FUV(8328, 8, 160)FUV(8329, 16, 160)FUV(8330, 24, 160)
			FUV(8331, 32, 160)FUV(8355, 40, 160)FUV(8356, 48, 160)FUV(8359, 56, 160)FUV(8362, 64, 160)FUV(8364, 72, 160)FUV(8453, 80, 160)
			FUV(8467, 88, 160)FUV(8470, 96, 160)FUV(8482, 104, 160)FUV(8486, 112, 160)FUV(8494, 120, 160)FUV(8528, 128, 160)FUV(8529, 136, 160)
			FUV(8531, 144, 160)FUV(8532, 152, 160)FUV(8533, 160, 160)FUV(8534, 168, 160)FUV(8535, 176, 160)FUV(8536, 184, 160)FUV(8537, 192, 160)
			FUV(8538, 200, 160)FUV(8539, 208, 160)FUV(8540, 216, 160)FUV(8541, 224, 160)FUV(8542, 232, 160)FUV(8592, 240, 160)FUV(8593, 248, 160)
			FUV(8594, 0, 168)FUV(8595, 8, 168)FUV(8596, 16, 168)FUV(8597, 24, 168)FUV(8616, 32, 168)FUV(8706, 40, 168)FUV(8709, 48, 168)
			FUV(8710, 56, 168)FUV(8712, 64, 168)FUV(8719, 72, 168)FUV(8721, 80, 168)FUV(8722, 88, 168)FUV(8725, 96, 168)FUV(8729, 104, 168)
			FUV(8730, 112, 168)FUV(8734, 120, 168)FUV(8735, 128, 168)FUV(8745, 136, 168)FUV(8747, 144, 168)FUV(8776, 152, 168)FUV(8800, 160, 168)
			FUV(8801, 168, 168)FUV(8804, 176, 168)FUV(8805, 184, 168)FUV(8857, 192, 168)FUV(8960, 200, 168)FUV(8962, 208, 168)FUV(8976, 216, 168)
			FUV(8992, 224, 168)FUV(8993, 232, 168)FUV(9472, 240, 168)FUV(9474, 248, 168)FUV(9484, 0, 176)FUV(9488, 8, 176)FUV(9492, 16, 176)
			FUV(9496, 24, 176)FUV(9500, 32, 176)FUV(9508, 40, 176)FUV(9516, 48, 176)FUV(9524, 56, 176)FUV(9532, 64, 176)FUV(9552, 72, 176)
			FUV(9553, 80, 176)FUV(9554, 88, 176)FUV(9555, 96, 176)FUV(9556, 104, 176)FUV(9557, 112, 176)FUV(9558, 120, 176)FUV(9559, 128, 176)
			FUV(9560, 136, 176)FUV(9561, 144, 176)FUV(9562, 152, 176)FUV(9563, 160, 176)FUV(9564, 168, 176)FUV(9565, 176, 176)FUV(9566, 184, 176)
			FUV(9567, 192, 176)FUV(9568, 200, 176)FUV(9569, 208, 176)FUV(9570, 216, 176)FUV(9571, 224, 176)FUV(9572, 232, 176)FUV(9573, 240, 176)
			FUV(9574, 248, 176)FUV(9575, 0, 184)FUV(9576, 8, 184)FUV(9577, 16, 184)FUV(9578, 24, 184)FUV(9579, 32, 184)FUV(9580, 40, 184)
			FUV(9600, 48, 184)FUV(9601, 56, 184)FUV(9604, 64, 184)FUV(9608, 72, 184)FUV(9612, 80, 184)FUV(9616, 88, 184)FUV(9617, 96, 184)
			FUV(9618, 104, 184)FUV(9619, 112, 184)FUV(9632, 120, 184)FUV(9633, 128, 184)FUV(9642, 136, 184)FUV(9643, 144, 184)FUV(9644, 152, 184)
			FUV(9650, 160, 184)FUV(9658, 168, 184)FUV(9660, 176, 184)FUV(9668, 184, 184)FUV(9674, 192, 184)FUV(9675, 200, 184)FUV(9679, 208, 184)
			FUV(9688, 216, 184)FUV(9689, 224, 184)FUV(9702, 232, 184)FUV(9786, 240, 184)FUV(9787, 248, 184)FUV(9788, 0, 192)FUV(9792, 8, 192)
			FUV(9794, 16, 192)FUV(9824, 24, 192)FUV(9827, 32, 192)FUV(9829, 40, 192)FUV(9830, 48, 192)FUV(9834, 56, 192)FUV(9835, 64, 192)
			FUV(10003, 72, 192)FUV(64257, 80, 192)FUV(64258, 88, 192)FUV(65533, 96, 192) default: *x = 96; *y = 192; break;
	}
}



void olcConsoleGameEngineGLOOP::UpdateMousePosition(int x, int y)
{
	float fx = (x - m_fDrawOffsetX) / (m_nScreenWidth * m_nFontWidth * m_fDrawScale);
	float fy = (y - m_fDrawOffsetY) / (m_nScreenHeight * m_nFontHeight * m_fDrawScale);

	fx = fx < 0 ? 0 : fx > 1.0f ? 1.0f : fx;
	fy = fy < 0 ? 0 : fy > 1.0f ? 1.0f : fy;

	m_mousePosX = (int)(fx * m_nScreenWidth);
	m_mousePosY = (int)(fy * m_nScreenHeight);
}

void olcConsoleGameEngineGLOOP::ToggleFullscreen(HWND hWnd)
{
	static WINDOWPLACEMENT prev = { sizeof(WINDOWPLACEMENT) };

	DWORD style = GetWindowLong(hWnd, GWL_STYLE);

	if (style & WS_OVERLAPPEDWINDOW)
	{
		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		if (GetWindowPlacement(hWnd, &prev))
		{
			SetWindowLong(hWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
			SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
	}
	else
	{
		SetWindowLong(hWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(hWnd, &prev);
		SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

void olcConsoleGameEngineGLOOP::WindowResize(void)
{
	DWORD style = GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD stylex = GetWindowLong(m_hWnd, GWL_EXSTYLE);

	RECT rWndRect = { 0, 0, m_nWindowWidth, m_nWindowHeight };

	if (style & WS_OVERLAPPEDWINDOW)
	{
		AdjustWindowRectEx(&rWndRect, style, FALSE, stylex);

		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;

		SetWindowPos(m_hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER);
	}
	else
	{
		ToggleFullscreen(m_hWnd);

		AdjustWindowRectEx(&rWndRect, style, FALSE, stylex);

		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;

		SetWindowPos(m_hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOOWNERZORDER);

		ToggleFullscreen(m_hWnd);
	}

}

void olcConsoleGameEngineGLOOP::WindowUpdateScale(void)
{
	int width = m_nWindowWidth;
	int height = m_nWindowHeight;

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, -1, 100);
	glMatrixMode(GL_MODELVIEW);

	float scaleX = (float)width / (float)(m_nScreenWidth * m_nFontWidth);
	float scaleY = (float)height / (float)(m_nScreenHeight * m_nFontHeight);

	if (scaleX < scaleY)
	{
		m_fDrawScale = scaleX;
		m_fDrawOffsetX = 0;
		m_fDrawOffsetY = ((float)height - (float)(m_nScreenHeight * m_nFontHeight * scaleX)) * 0.5f;
	}
	else
	{
		m_fDrawScale = scaleY;
		m_fDrawOffsetX = ((float)width - (float)(m_nScreenWidth * m_nFontWidth * scaleY)) * 0.5f;
		m_fDrawOffsetY = 0;
	}
}

int olcConsoleGameEngineGLOOP::SetPixelFormatGL(void)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int pf = ChoosePixelFormat(m_hDevCtx, &pfd);
	if (!pf) return 0;

	return SetPixelFormat(m_hDevCtx, pf, &pfd);
}


LRESULT CALLBACK olcConsoleGameEngineGLOOP::olcWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static olcConsoleGameEngineGLOOP *cge;


	switch (uMsg)
	{
	case WM_CREATE:
		cge = static_cast<olcConsoleGameEngineGLOOP*>(((LPCREATESTRUCT)lParam)->lpCreateParams);

		cge->m_hDevCtx = GetDC(hWnd);
		if (!cge->SetPixelFormatGL()) return -1;

		cge->m_hRenCtx = wglCreateContext(cge->m_hDevCtx);
		if (!cge->m_hRenCtx) return -1;

		wglMakeCurrent(cge->m_hDevCtx, cge->m_hRenCtx);
		ShowWindow(cge->m_hConsole, SW_HIDE);
		return 0;

	case WM_SYSCHAR:
		//ding ding ding
		return 0;

	case WM_MOUSEMOVE:
		cge->UpdateMousePosition(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_SIZE:
		cge->m_nWindowWidth = LOWORD(lParam);
		cge->m_nWindowHeight = HIWORD(lParam);
		cge->m_bDoWindowUpdate = true;
		return 0;

	case WM_SETFOCUS:
		cge->m_bConsoleInFocus = true;
		return 0;

	case WM_KILLFOCUS:
		cge->m_bConsoleInFocus = false;
		return 0;

	case WM_CLOSE:
		m_bAtomActive = false;
		return 0;

	case WM_DESTROY:
		ShowWindow(cge->m_hConsole, SW_SHOW);
		PostQuitMessage(0);
		return 0;

	case 0x8000:
		cge->ToggleFullscreen(hWnd);
		return 0;

	case 0x8001:
		cge->WindowResize();
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND olcConsoleGameEngineGLOOP::ConstructWindow(int width, int height)
{
	wchar_t wnd_title[] = L"OneLoneCoder.com - Console Game Engine (OGL)";
	wchar_t wnd_class[] = L"OLC_CONSOLE_GAME_ENGINE_CLASS";

	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASS wc =
	{
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		olcWndProc,
		0,
		0,
		hInstance,
		LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW),
		NULL,
		NULL,
		wnd_class
	};

	RegisterClass(&wc);

	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES;
	DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	RECT rWndRect = { 0, 0, width, height };

	AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);

	width = rWndRect.right - rWndRect.left;
	height = rWndRect.bottom - rWndRect.top;

	return CreateWindowEx(dwExStyle, wnd_class, wnd_title, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, hInstance, this);
}


olcConsoleGameEngineGLOOP::olcConsoleGameEngineGLOOP()
{
	m_nScreenWidth = 80;
	m_nScreenHeight = 30;

	memset(m_keyNewState, 0, 256 * sizeof(short));
	memset(m_keyOldState, 0, 256 * sizeof(short));
	memset(m_keys, 0, 256 * sizeof(sKeyState));
	m_mousePosX = 0;
	m_mousePosY = 0;

	m_bEnableSound = false;

	m_sAppName = L"Default";

	//grab 1 GB or memory
	m_bufMemory = (uint8_t*)VirtualAlloc(NULL, 1024 * 1024 * 1024, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!m_bufMemory) throw exception("No Memory!");

	m_bufScreen = (CHAR_INFO*)&m_bufMemory[0];
	m_bufScreen_old = (CHAR_INFO*)&m_bufMemory[21474304];

	m_fVertexArray = (float*)&m_bufMemory[42948608];
	m_fTexCoordArray = (float*)&m_bufMemory[300640256];
	m_uIndicesArray = (uint32_t*)&m_bufMemory[558331904];

	m_uForegroundColorArray = (uint32_t*)&m_bufMemory[816023552];
	m_uBackgroundColorArray = (uint32_t*)&m_bufMemory[944869376];

	m_hConsole = GetConsoleWindow();
}

void olcConsoleGameEngineGLOOP::EnableSound()
{
	m_bEnableSound = true;
}

int olcConsoleGameEngineGLOOP::ConstructConsole(int width, int height, int fontw, int fonth)
{
	m_nScreenWidth = width;
	m_nScreenHeight = height;

	m_nFontWidth = fontw;
	m_nFontHeight = fonth;

	int newWndWidth = width * fontw;
	int newWndHeight = height * fonth;

	if (m_hWnd && ((m_nWindowWidth != newWndWidth) || (m_nWindowHeight != newWndHeight)))
	{
		SendMessage(m_hWnd, 0x8001, 0, 0);
	}

	m_nWindowWidth = newWndWidth;
	m_nWindowHeight = newWndHeight;

	// Allocate memory for screen buffer
	size_t bufLen = m_nScreenWidth * m_nScreenHeight;

	if (bufLen > 0x51EB00)
	{
		MessageBoxA(NULL, "Not enough memory!", "ERROR!", MB_OK);
		ExitProcess(0xDEADC0DE);
	}

	memset(m_bufMemory, 0, bufLen * 200);

	for (int y = 0; y < m_nScreenHeight; y++)
		for (int x = 0; x < m_nScreenWidth; x++)
		{
			int pos = y * m_nScreenWidth + x;

			float x1 = (float)(x);
			float y1 = (float)(y);
			float x2 = (float)(x + 1);
			float y2 = (float)(y + 1);

			pos *= 12;

			m_fVertexArray[pos + 0] = x1;
			m_fVertexArray[pos + 1] = y1;
			m_fVertexArray[pos + 2] = x2;
			m_fVertexArray[pos + 3] = y1;
			m_fVertexArray[pos + 4] = x1;
			m_fVertexArray[pos + 5] = y2;
			m_fVertexArray[pos + 6] = x2;
			m_fVertexArray[pos + 7] = y1;
			m_fVertexArray[pos + 8] = x1;
			m_fVertexArray[pos + 9] = y2;
			m_fVertexArray[pos + 10] = x2;
			m_fVertexArray[pos + 11] = y2;

			m_uIndicesArray[pos + 0] = pos + 0;
			m_uIndicesArray[pos + 1] = pos + 1;
			m_uIndicesArray[pos + 2] = pos + 2;
			m_uIndicesArray[pos + 3] = pos + 3;
			m_uIndicesArray[pos + 4] = pos + 4;
			m_uIndicesArray[pos + 5] = pos + 5;
			m_uIndicesArray[pos + 6] = pos + 6;
			m_uIndicesArray[pos + 7] = pos + 7;
			m_uIndicesArray[pos + 8] = pos + 8;
			m_uIndicesArray[pos + 9] = pos + 9;
			m_uIndicesArray[pos + 10] = pos + 10;
			m_uIndicesArray[pos + 11] = pos + 11;
		}

	return 1;
}

void olcConsoleGameEngineGLOOP::Draw(int x, int y, wchar_t c, short col)
{
	if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
	{
		m_bufScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
		m_bufScreen[y * m_nScreenWidth + x].Attributes = col;
	}
}

void olcConsoleGameEngineGLOOP::Fill(int x1, int y1, int x2, int y2, wchar_t c, short col)
{
	Clip(x1, y1);
	Clip(x2, y2);
	for (int x = x1; x < x2; x++)
		for (int y = y1; y < y2; y++)
			Draw(x, y, c, col);
}

void olcConsoleGameEngineGLOOP::DrawString(int x, int y, wstring c, short col)
{
	for (size_t i = 0; i < c.size(); i++)
	{
		m_bufScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = c[i];
		m_bufScreen[y * m_nScreenWidth + x + i].Attributes = col;
	}
}

void olcConsoleGameEngineGLOOP::DrawStringAlpha(int x, int y, wstring c, short col)
{
	for (size_t i = 0; i < c.size(); i++)
	{
		if (c[i] != L' ')
		{
			m_bufScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = c[i];
			m_bufScreen[y * m_nScreenWidth + x + i].Attributes = col;
		}
	}
}

void olcConsoleGameEngineGLOOP::Clip(int &x, int &y)
{
	if (x < 0) x = 0;
	if (x >= m_nScreenWidth) x = m_nScreenWidth;
	if (y < 0) y = 0;
	if (y >= m_nScreenHeight) y = m_nScreenHeight;
}

void olcConsoleGameEngineGLOOP::DrawLine(int x1, int y1, int x2, int y2, wchar_t c, short col)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1;
	dy = y2 - y1;
	dx1 = abs(dx);
	dy1 = abs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;
	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1;
			y = y1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}
		Draw(x, y, c, col);
		for (i = 0; x<xe; i++)
		{
			x = x + 1;
			if (px<0)
				px = px + 2 * dy1;
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0))
					y = y + 1;
				else
					y = y - 1;
				px = px + 2 * (dy1 - dx1);
			}
			Draw(x, y, c, col);
		}
	}
	else
	{
		if (dy >= 0)
		{
			x = x1;
			y = y1;
			ye = y2;
		}
		else
		{
			x = x2;
			y = y2;
			ye = y1;
		}
		Draw(x, y, c, col);
		for (i = 0; y<ye; i++)
		{
			y = y + 1;
			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx<0 && dy<0) || (dx>0 && dy>0))
					x = x + 1;
				else
					x = x - 1;
				py = py + 2 * (dx1 - dy1);
			}
			Draw(x, y, c, col);
		}
	}
}

void olcConsoleGameEngineGLOOP::DrawCircle(int xc, int yc, int r, wchar_t c, short col)
{
	int x = 0;
	int y = r;
	int p = 3 - 2 * r;
	if (!r) return;

	while (y >= x) // only formulate 1/8 of circle
	{
		Draw(xc - x, yc - y, c, col);//upper left left
		Draw(xc - y, yc - x, c, col);//upper upper left
		Draw(xc + y, yc - x, c, col);//upper upper right
		Draw(xc + x, yc - y, c, col);//upper right right
		Draw(xc - x, yc + y, c, col);//lower left left
		Draw(xc - y, yc + x, c, col);//lower lower left
		Draw(xc + y, yc + x, c, col);//lower lower right
		Draw(xc + x, yc + y, c, col);//lower right right
		if (p < 0) p += 4 * x++ + 6;
		else p += 4 * (x++ - y--) + 10;
	}
}

void olcConsoleGameEngineGLOOP::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
{
	DrawLine(x1, y1, x2, y2, c, col);
	DrawLine(x2, y2, x3, y3, c, col);
	DrawLine(x3, y3, x1, y1, c, col);
}

// https://www.avrfreaks.net/sites/default/files/triangles.c
void olcConsoleGameEngineGLOOP::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
{
	auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
	auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, c, col); };

	int t1x, t2x, y, minx, maxx, t1xp, t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;
	// Sort vertices
	if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
	if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
	if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
	else signx2 = 1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
		SWAP(dx1, dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
		SWAP(dy2, dx2);
		changed2 = true;
	}

	e2 = (int)(dx2 >> 1);
	// Flat top, just process the second half
	if (y1 == y2) goto next;
	e1 = (int)(dx1 >> 1);

	for (int i = 0; i < dx1;) {
		t1xp = 0; t2xp = 0;
		if (t1x<t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i<dx1) {
			i++;
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) t1xp = signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
		// Move line
	next1:
		// process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
		if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
		drawline(minx, maxx, y);    // Draw line from min to max points found on the y
									// Now increase y
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y == y2) break;

	}
next:
	// Second half
	dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
	else signx1 = 1;
	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1) {   // swap values
		SWAP(dy1, dx1);
		changed1 = true;
	}
	else changed1 = false;

	e1 = (int)(dx1 >> 1);

	for (int i = 0; i <= dx1; i++) {
		t1xp = 0; t2xp = 0;
		if (t1x<t2x) { minx = t1x; maxx = t2x; }
		else { minx = t2x; maxx = t1x; }
		// process first line until y value is about to change
		while (i<dx1) {
			e1 += dy1;
			while (e1 >= dx1) {
				e1 -= dx1;
				if (changed1) { t1xp = signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if (i<dx1) i++;
		}
	next3:
		// process second line until y value is about to change
		while (t2x != x3) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp = signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next4:

		if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
		if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
		drawline(minx, maxx, y);
		if (!changed1) t1x += signx1;
		t1x += t1xp;
		if (!changed2) t2x += signx2;
		t2x += t2xp;
		y += 1;
		if (y>y3) return;
	}
}

void olcConsoleGameEngineGLOOP::FillCircle(int xc, int yc, int r, wchar_t c, short col)
{
	// Taken from wikipedia
	int x = 0;
	int y = r;
	int p = 3 - 2 * r;
	if (!r) return;

	auto drawline = [&](int sx, int ex, int ny)
	{
		for (int i = sx; i <= ex; i++)
			Draw(i, ny, c, col);
	};

	while (y >= x)
	{
		// Modified to draw scan-lines instead of edges
		drawline(xc - x, xc + x, yc - y);
		drawline(xc - y, xc + y, yc - x);
		drawline(xc - x, xc + x, yc + y);
		drawline(xc - y, xc + y, yc + x);
		if (p < 0) p += 4 * x++ + 6;
		else p += 4 * (x++ - y--) + 10;
	}
};

void olcConsoleGameEngineGLOOP::DrawSprite(int x, int y, olcSprite *sprite)
{
	if (sprite == nullptr)
		return;

	for (int i = 0; i < sprite->nWidth; i++)
	{
		for (int j = 0; j < sprite->nHeight; j++)
		{
			if (sprite->GetGlyph(i, j) != L' ')
				Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
		}
	}
}

void olcConsoleGameEngineGLOOP::DrawPartialSprite(int x, int y, olcSprite *sprite, int ox, int oy, int w, int h)
{
	if (sprite == nullptr)
		return;

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			if (sprite->GetGlyph(i + ox, j + oy) != L' ')
				Draw(x + i, y + j, sprite->GetGlyph(i + ox, j + oy), sprite->GetColour(i + ox, j + oy));
		}
	}
}

void olcConsoleGameEngineGLOOP::DrawWireFrameModel(const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r, float s, short col, short c)
{
	// pair.first = x coordinate
	// pair.second = y coordinate

	// Create translated model vector of coordinate pairs
	vector<pair<float, float>> vecTransformedCoordinates;
	int verts = vecModelCoordinates.size();
	vecTransformedCoordinates.resize(verts);

	// Rotate
	for (int i = 0; i < verts; i++)
	{
		vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
		vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
	}

	// Scale
	for (int i = 0; i < verts; i++)
	{
		vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
		vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
	}

	// Translate
	for (int i = 0; i < verts; i++)
	{
		vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
		vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
	}

	// Draw Closed Polygon
	for (int i = 0; i < verts + 1; i++)
	{
		int j = (i + 1);
		DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
			(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, c, col);
	}
}

olcConsoleGameEngineGLOOP::~olcConsoleGameEngineGLOOP()
{
	if (m_bufMemory) VirtualFree(m_bufMemory, 0, MEM_RELEASE);

	m_bufMemory = nullptr;

	m_bufScreen = nullptr;
	m_bufScreen_old = nullptr;

	m_fVertexArray = nullptr;
	m_fTexCoordArray = nullptr;
	m_uIndicesArray = nullptr;

	m_uForegroundColorArray = nullptr;
	m_uBackgroundColorArray = nullptr;
}

void olcConsoleGameEngineGLOOP::GenerateMipmapPow2(uint8_t *tex_new, uint8_t *tex_old, uint8_t *ref_alpha, int size)
{
	for (int y = 0; y < size; y++)
		for (int x = 0; x < size; x++)
		{
			uint8_t *p0 = &tex_old[(y << 1) * (size << 1) + (x << 1)];
			uint8_t *p1 = &p0[(size << 1)];

			tex_new[y * size + x] = (p0[0] + p0[1] + p1[0] + p1[1]) >> 2;
		}

	int char_size = size >> 5;

	for (int i = 0; i < 1024; i++)
	{
		int alpha = 0;

		int posy = (i >> 5) * char_size;
		int posx = (i & 0x1F) * char_size;

		for (int y = 0; y < char_size; y++)
			for (int x = 0; x < char_size; x++)
			{
				alpha += tex_new[(posy + y) * size + (posx + x)];
			}

		alpha /= char_size * char_size;

		float factor = (float)ref_alpha[i] / (float)alpha;

		for (int y = 0; y < char_size; y++)
			for (int x = 0; x < char_size; x++)
			{
				int value = (int)((float)(tex_new[(posy + y) * size + (posx + x)]) * factor);
				tex_new[(posy + y) * size + (posx + x)] = value > 255 ? 255 : value;
			}
	}

}


void olcConsoleGameEngineGLOOP::Start()
{
	m_bAtomActive = true;

	m_hWnd = ConstructWindow(m_nWindowWidth, m_nWindowHeight);
	if (!m_hWnd)
	{
		Error(L"Could not create GL window");
		return;
	}

	glGenTextures(1, &m_uFontTexture);
	glBindTexture(GL_TEXTURE_2D, m_uFontTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	//mipmap generation
	{
		font_decode_custom_base64(); // fill pxplus_ibm_cga

		glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, 256, 256, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, pxplus_ibm_cga);

		uint8_t *glyph_alpha = new uint8_t[1024]; // 32 * 32

		for (int i = 0; i < 1024; i++)
		{
			int alpha = 0;

			int posy = (i >> 5) << 3;
			int posx = (i & 0x1F) << 3;

			for (int y = 0; y < 8; y++)
				for (int x = 0; x < 8; x++)
				{
					alpha += pxplus_ibm_cga[(posy + y) * 256 + (posx + x)];
				}

			glyph_alpha[i] = (uint8_t)(alpha >> 6);
		}

		int texsize = 128;
		uint8_t *texbuf = new uint8_t[texsize * texsize];

		GenerateMipmapPow2(texbuf, pxplus_ibm_cga, glyph_alpha, texsize);
		glTexImage2D(GL_TEXTURE_2D, 1, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);

		texsize = 64;
		GenerateMipmapPow2(texbuf, texbuf, glyph_alpha, texsize);
		glTexImage2D(GL_TEXTURE_2D, 2, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);

		texsize = 32;
		GenerateMipmapPow2(texbuf, texbuf, glyph_alpha, texsize);
		glTexImage2D(GL_TEXTURE_2D, 3, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);

		texsize = 16;
		for (int level = 4; level < 9; level++)
		{
			for (int y = 0; y < texsize; y++)
				for (int x = 0; x < texsize; x++)
				{
					uint8_t *p0 = &texbuf[(y << 1) * (texsize << 1) + (x << 1)];
					uint8_t *p1 = &p0[texsize << 1];

					texbuf[y * texsize + x] = (p0[0] + p0[1] + p1[0] + p1[1]) >> 2;
				}

			glTexImage2D(GL_TEXTURE_2D, level, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
			texsize >>= 1;
		}

		texsize = 1;
		for (int level = 9; level < 1000; level++)
		{
			glTexImage2D(GL_TEXTURE_2D, level, GL_INTENSITY, texsize, texsize, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texbuf);
		}

		delete[] texbuf;
		delete[] glyph_alpha;
	}

	wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapInterval(0);

	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	wglMakeCurrent(NULL, NULL);

	// Star the thread
	thread t = thread(&olcConsoleGameEngineGLOOP::GameThread, this);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Wait for thread to be exited
	t.join();
}

void olcConsoleGameEngineGLOOP::GameThread()
{
	wglMakeCurrent(m_hDevCtx, m_hRenCtx);

	// Create user resources as part of this thread
	if (!OnUserCreate())
		m_bAtomActive = false;

	// Check if sound system should be enabled
	if (m_bEnableSound)
	{
		if (!CreateAudio())
		{
			m_bAtomActive = false; // Failed to create audio system			
			m_bEnableSound = false;
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, m_fVertexArray);
	glTexCoordPointer(2, GL_FLOAT, 0, m_fTexCoordArray);

	LARGE_INTEGER timeFreq, timeNew, timeOld;
	QueryPerformanceFrequency(&timeFreq);
	QueryPerformanceCounter(&timeOld);
	QueryPerformanceCounter(&timeNew);

	int nFrameCounter = 0;
	float fFrameTimeAccum = 0;

	while (m_bAtomActive)
	{
		// Run as fast as possible
		while (m_bAtomActive)
		{
			QueryPerformanceCounter(&timeNew);
			float fElapsedTime = (float)((timeNew.QuadPart - timeOld.QuadPart) / (double)timeFreq.QuadPart);
			timeOld = timeNew;

			for (int i = 0; i < 256; i++)
			{
				m_keyNewState[i] = GetAsyncKeyState(i) >> 15;

				m_keys[i].bPressed = false;
				m_keys[i].bReleased = false;

				if (m_keyNewState[i] != m_keyOldState[i])
				{
					if (m_keyNewState[i])
					{
						m_keys[i].bPressed = true;
						m_keys[i].bHeld = true;
					}
					else
					{
						m_keys[i].bReleased = true;
						m_keys[i].bHeld = false;
					}
				}

				m_keyOldState[i] = m_keyNewState[i];
			}

			m_mouse[0x00] = m_keys[VK_LBUTTON];
			m_mouse[0x01] = m_keys[VK_RBUTTON];
			m_mouse[0x02] = m_keys[VK_MBUTTON];
			m_mouse[0x03] = m_keys[0x05]; // VK_XBUTTON1
			m_mouse[0x04] = m_keys[0x06]; // VK_XBUTTON2

			if (m_keys[VK_MENU].bHeld && m_keys[VK_RETURN].bPressed)
			{
				SendMessage(m_hWnd, 0x8000, 0, 0);
			}

			if (m_bDoWindowUpdate)
			{
				WindowUpdateScale();
				m_bDoWindowUpdate = false;
			}

			glClear(GL_COLOR_BUFFER_BIT);

			// Handle Frame Update
			if (!OnUserUpdate(fElapsedTime))
			{
				m_bAtomActive = false;
				break;
			}

			// draw the things
			glPushMatrix();
			glTranslatef(m_fDrawOffsetX, m_fDrawOffsetY, 0.0f);
			glScalef(m_fDrawScale * m_nFontWidth, m_fDrawScale * m_nFontHeight, 1.0f);

			for (int y = 0; y < m_nScreenHeight; y++)
				for (int x = 0; x < m_nScreenWidth; x++)
				{
					int pos = y * m_nScreenWidth + x;

					if ((m_bufScreen[pos].Char.UnicodeChar == m_bufScreen_old[pos].Char.UnicodeChar) &&
						(m_bufScreen[pos].Attributes == m_bufScreen_old[pos].Attributes))
						continue;

					m_bufScreen_old[pos] = m_bufScreen[pos];

					WCHAR id = m_bufScreen[pos].Char.UnicodeChar;
					WORD col = m_bufScreen[pos].Attributes;

					int u, v;
					float u1, v1, u2, v2;
					uint32_t fg, bg;

					if (id == L' ')
					{
						u1 = u2 = v1 = v2 = 0.0f;
						fg = bg = 0;
					}
					else
					{
						GetFontCoords(id, &u, &v);

						u1 = (u) / 256.0f;
						v1 = (v) / 256.0f;
						u2 = (u + 8) / 256.0f;
						v2 = (v + 8) / 256.0f;

						fg = m_ColourPalette[col & 0xF];
						bg = m_ColourPalette[(col >> 4) & 0xF];
					}

					pos *= 6;

					m_uForegroundColorArray[pos + 0] = fg;
					m_uForegroundColorArray[pos + 1] = fg;
					m_uForegroundColorArray[pos + 2] = fg;
					m_uForegroundColorArray[pos + 3] = fg;
					m_uForegroundColorArray[pos + 4] = fg;
					m_uForegroundColorArray[pos + 5] = fg;

					m_uBackgroundColorArray[pos + 0] = bg;
					m_uBackgroundColorArray[pos + 1] = bg;
					m_uBackgroundColorArray[pos + 2] = bg;
					m_uBackgroundColorArray[pos + 3] = bg;
					m_uBackgroundColorArray[pos + 4] = bg;
					m_uBackgroundColorArray[pos + 5] = bg;

					pos *= 2;

					m_fTexCoordArray[pos + 0] = u1;
					m_fTexCoordArray[pos + 1] = v1;
					m_fTexCoordArray[pos + 2] = u2;
					m_fTexCoordArray[pos + 3] = v1;
					m_fTexCoordArray[pos + 4] = u1;
					m_fTexCoordArray[pos + 5] = v2;
					m_fTexCoordArray[pos + 6] = u2;
					m_fTexCoordArray[pos + 7] = v1;
					m_fTexCoordArray[pos + 8] = u1;
					m_fTexCoordArray[pos + 9] = v2;
					m_fTexCoordArray[pos + 10] = u2;
					m_fTexCoordArray[pos + 11] = v2;
				}

			glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_uBackgroundColorArray);
			glDrawArrays(GL_TRIANGLES, 0, m_nScreenWidth * m_nScreenHeight * 6);
			//glDrawElements(GL_TRIANGLES, m_nScreenWidth * m_nScreenHeight * 6, GL_UNSIGNED_INT, m_uIndicesArray);

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);

			glColorPointer(4, GL_UNSIGNED_BYTE, 0, m_uForegroundColorArray);
			glDrawArrays(GL_TRIANGLES, 0, m_nScreenWidth * m_nScreenHeight * 6);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			glPopMatrix();

			// Update Title & Present Screen Buffer
			wchar_t sNewTitle[256];
			swprintf_s(sNewTitle, 256, L"OneLoneCoder.com - Console Game Engine (OGL) - %s - FPS: %3.2f", m_sAppName.c_str(), 1.0f / fElapsedTime);
			SetWindowText(m_hWnd, sNewTitle);

			SwapBuffers(m_hDevCtx);
		}

		if (m_bEnableSound)
		{
			// Close and Clean up audio system
		}

		if (OnUserDestroy())
		{
			// User has permitted destroy, so exit and clean up
			m_cvGameFinished.notify_one();
		}
		else
		{
			// User denied destroy for some reason, so continue running
			m_bAtomActive = true;
		}
	}

	PostMessage(m_hWnd, WM_DESTROY, 0, 0);
}



int olcConsoleGameEngineGLOOP::Error(const wchar_t *msg)
{
	wchar_t buff1[256];
	wchar_t buff2[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, buff1, 256, NULL);
	wsprintf(buff2, L"%s\n\n%s", msg, buff1);
	MessageBox(NULL, buff2, L"ERROR", MB_ICONERROR | MB_OK);
	return 0;
}



// Load a 16-bit WAVE file @ 44100Hz ONLY into memory. A sample ID
// number is returned if successful, otherwise -1
unsigned int olcConsoleGameEngineGLOOP::LoadAudioSample(std::wstring sWavFile)
{
	if (!m_bEnableSound)
		return -1;

	olcAudioSample a(sWavFile);
	if (a.bSampleValid)
	{
		vecAudioSamples.push_back(a);
		return vecAudioSamples.size();
	}
	else
		return -1;
}

// Add sample 'id' to the mixers sounds to play list
void olcConsoleGameEngineGLOOP::PlaySample(int id, bool bLoop = false)
{
	sCurrentlyPlayingSample a;
	a.nAudioSampleID = id;
	a.nSamplePosition = 0;
	a.bFinished = false;
	a.bLoop = bLoop;
	listActiveSamples.push_back(a);
}

void olcConsoleGameEngineGLOOP::StopSample(int id)
{

}

// The audio system uses by default a specific wave format
bool olcConsoleGameEngineGLOOP::CreateAudio(unsigned int nSampleRate, unsigned int nChannels,
	unsigned int nBlocks, unsigned int nBlockSamples)
{
	// Initialise Sound Engine
	m_bAudioThreadActive = false;
	m_nSampleRate = nSampleRate;
	m_nChannels = nChannels;
	m_nBlockCount = nBlocks;
	m_nBlockSamples = nBlockSamples;
	m_nBlockFree = m_nBlockCount;
	m_nBlockCurrent = 0;
	m_pBlockMemory = nullptr;
	m_pWaveHeaders = nullptr;

	// Device is available
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = m_nSampleRate;
	waveFormat.wBitsPerSample = sizeof(short) * 8;
	waveFormat.nChannels = m_nChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Open Device if valid
	if (waveOutOpen(&m_hwDevice, WAVE_MAPPER, &waveFormat, (DWORD_PTR)waveOutProcWrap, (DWORD_PTR)this, CALLBACK_FUNCTION) != S_OK)
		return DestroyAudio();

	// Allocate Wave|Block Memory
	m_pBlockMemory = new short[m_nBlockCount * m_nBlockSamples];
	if (m_pBlockMemory == nullptr)
		return DestroyAudio();
	ZeroMemory(m_pBlockMemory, sizeof(short) * m_nBlockCount * m_nBlockSamples);

	m_pWaveHeaders = new WAVEHDR[m_nBlockCount];
	if (m_pWaveHeaders == nullptr)
		return DestroyAudio();
	ZeroMemory(m_pWaveHeaders, sizeof(WAVEHDR) * m_nBlockCount);

	// Link headers to block memory
	for (unsigned int n = 0; n < m_nBlockCount; n++)
	{
		m_pWaveHeaders[n].dwBufferLength = m_nBlockSamples * sizeof(short);
		m_pWaveHeaders[n].lpData = (LPSTR)(m_pBlockMemory + (n * m_nBlockSamples));
	}

	m_bAudioThreadActive = true;
	m_AudioThread = std::thread(&olcConsoleGameEngineGLOOP::AudioThread, this);

	// Start the ball rolling with the sound delivery thread
	std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
	m_cvBlockNotZero.notify_one();
	return true;
}

// Stop and clean up audio system
bool olcConsoleGameEngineGLOOP::DestroyAudio()
{
	m_bAudioThreadActive = false;
	return false;
}

// Handler for soundcard request for more data
void olcConsoleGameEngineGLOOP::waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg != WOM_DONE) return;
	m_nBlockFree++;
	std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
	m_cvBlockNotZero.notify_one();
}

// Static wrapper for sound card handler
void CALLBACK olcConsoleGameEngineGLOOP::waveOutProcWrap(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	((olcConsoleGameEngineGLOOP*)dwInstance)->waveOutProc(hWaveOut, uMsg, dwParam1, dwParam2);
}

// Audio thread. This loop responds to requests from the soundcard to fill 'blocks'
// with audio data. If no requests are available it goes dormant until the sound
// card is ready for more data. The block is fille by the "user" in some manner
// and then issued to the soundcard.
void olcConsoleGameEngineGLOOP::AudioThread()
{
	m_fGlobalTime = 0.0f;
	float fTimeStep = 1.0f / (float)m_nSampleRate;

	// Goofy hack to get maximum integer for a type at run-time
	short nMaxSample = (short)pow(2, (sizeof(short) * 8) - 1) - 1;
	float fMaxSample = (float)nMaxSample;
	short nPreviousSample = 0;

	while (m_bAudioThreadActive)
	{
		// Wait for block to become available
		if (m_nBlockFree == 0)
		{
			std::unique_lock<std::mutex> lm(m_muxBlockNotZero);
			while (m_nBlockFree == 0) // sometimes, Windows signals incorrectly
				m_cvBlockNotZero.wait(lm);
		}

		// Block is here, so use it
		m_nBlockFree--;

		// Prepare block for processing
		if (m_pWaveHeaders[m_nBlockCurrent].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));

		short nNewSample = 0;
		int nCurrentBlock = m_nBlockCurrent * m_nBlockSamples;

		auto clip = [](float fSample, float fMax)
		{
			if (fSample >= 0.0)
				return fmin(fSample, fMax);
			else
				return fmax(fSample, -fMax);
		};

		for (unsigned int n = 0; n < m_nBlockSamples; n += m_nChannels)
		{
			// User Process
			for (unsigned int c = 0; c < m_nChannels; c++)
			{
				nNewSample = (short)(clip(GetMixerOutput(c, m_fGlobalTime, fTimeStep), 1.0) * fMaxSample);
				m_pBlockMemory[nCurrentBlock + n + c] = nNewSample;
				nPreviousSample = nNewSample;
			}

			m_fGlobalTime = m_fGlobalTime + fTimeStep;
		}

		// Send block to sound device
		waveOutPrepareHeader(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
		waveOutWrite(m_hwDevice, &m_pWaveHeaders[m_nBlockCurrent], sizeof(WAVEHDR));
		m_nBlockCurrent++;
		m_nBlockCurrent %= m_nBlockCount;
	}
}

// Overridden by user if they want to generate sound in real-time
float olcConsoleGameEngineGLOOP::onUserSoundSample(int nChannel, float fGlobalTime, float fTimeStep)
{
	return 0.0f;
}

// Overriden by user if they want to manipulate the sound before it is played
float olcConsoleGameEngineGLOOP::onUserSoundFilter(int nChannel, float fGlobalTime, float fSample)
{
	return fSample;
}

// The Sound Mixer - If the user wants to play many sounds simultaneously, and
// perhaps the same sound overlapping itself, then you need a mixer, which
// takes input from all sound sources for that audio frame. This mixer maintains
// a list of sound locations for all concurrently playing audio samples. Instead
// of duplicating audio data, we simply store the fact that a sound sample is in
// use and an offset into its sample data. As time progresses we update this offset
// until it is beyound the length of the sound sample it is attached to. At this
// point we remove the playing souind from the list.
//
// Additionally, the users application may want to generate sound instead of just
// playing audio clips (think a synthesizer for example) in whcih case we also
// provide an "onUser..." event to allow the user to return a sound for that point
// in time.
//
// Finally, before the sound is issued to the operating system for performing, the
// user gets one final chance to "filter" the sound, perhaps changing the volume
// or adding funky effects
float olcConsoleGameEngineGLOOP::GetMixerOutput(int nChannel, float fGlobalTime, float fTimeStep)
{
	// Accumulate sample for this channel
	float fMixerSample = 0.0f;

	for (auto &s : listActiveSamples)
	{
		// Calculate sample position
		s.nSamplePosition += (long)((float)vecAudioSamples[s.nAudioSampleID - 1].wavHeader.nSamplesPerSec * fTimeStep);

		// If sample position is valid add to the mix
		if (s.nSamplePosition < vecAudioSamples[s.nAudioSampleID - 1].nSamples)
			fMixerSample += vecAudioSamples[s.nAudioSampleID - 1].fSample[(s.nSamplePosition * vecAudioSamples[s.nAudioSampleID - 1].nChannels) + nChannel];
		else
			s.bFinished = true; // Else sound has completed
	}

	// If sounds have completed then remove them
	listActiveSamples.remove_if([](const sCurrentlyPlayingSample &s) {return s.bFinished; });

	// The users application might be generating sound, so grab that if it exists
	fMixerSample += onUserSoundSample(nChannel, fGlobalTime, fTimeStep);

	// Return the sample via an optional user override to filter the sound
	return onUserSoundFilter(nChannel, fGlobalTime, fMixerSample);
}


atomic<bool> olcConsoleGameEngineGLOOP::m_bAtomActive(false);
condition_variable olcConsoleGameEngineGLOOP::m_cvGameFinished;
mutex olcConsoleGameEngineGLOOP::m_muxGame;
