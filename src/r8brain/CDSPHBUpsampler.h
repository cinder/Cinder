//$ nobt
//$ nocpp

/**
 * @file CDSPHBUpsampler.h
 *
 * @brief Half-band upsampling class.
 *
 * This file includes half-band upsampling class.
 *
 * r8brain-free-src Copyright (c) 2013-2022 Aleksey Vaneev
 * See the "LICENSE" file for license.
 */

#ifndef R8B_CDSPHBUPSAMPLER_INCLUDED
#define R8B_CDSPHBUPSAMPLER_INCLUDED

#include "CDSPProcessor.h"

namespace r8b {

/**
 * @brief Half-band upsampling class.
 *
 * Class implements brute-force half-band 2X upsampling that uses small
 * sparse symmetric FIR filters. It is very efficient and should be used at
 * latter upsampling steps after initial steep 2X upsampling.
 */

class CDSPHBUpsampler : public CDSPProcessor
{
public:
	/**
	 * Function that provides filter data for various steepness indices and
	 * attenuations.
	 *
	 * @param ReqAtten Required half-band filter attentuation.
	 * @param SteepIndex Steepness index - 0=steepest. Corresponds to general
	 * upsampling/downsampling ratio, e.g. at 4x 0 is used, at 8x 1 is used,
	 * etc.
	 */

	static void getHBFilter( const double ReqAtten, const int SteepIndex,
		const double*& flt, int& fltt, double& att )
	{
		static const double HBKernel_4A[ 4 ] = { // -54.5176 dB, 4
			6.1729335650971517e-001, -1.5963945620743250e-001,
			5.5073370934086312e-002, -1.4603578989932850e-002,};
		static const double HBKernel_5A[ 5 ] = { // -66.3075 dB, 4
			6.2068807424902472e-001, -1.6827573634467302e-001,
			6.5263016720721170e-002, -2.2483331611592005e-002,
			5.2917326684281110e-003,};
		static const double HBKernel_6A[ 6 ] = { // -89.5271 dB, 4
			6.2187202340480707e-001, -1.7132842113816371e-001,
			6.9019169178765674e-002, -2.5799728312695277e-002,
			7.4880112525741666e-003, -1.2844465869952567e-003,};
		static const double HBKernel_7A[ 7 ] = { // -105.2842 dB, 4
			6.2354494135775851e-001, -1.7571220703702045e-001,
			7.4529843603968457e-002, -3.0701736822442153e-002,
			1.0716755639039573e-002, -2.7833422930759735e-003,
			4.1118797093875510e-004,};
		static const double HBKernel_8A[ 8 ] = { // -121.0063 dB, 4
			6.2488363107953926e-001, -1.7924942606514119e-001,
			7.9068155655640557e-002, -3.4907523415495731e-002,
			1.3710256799907897e-002, -4.3991142586987933e-003,
			1.0259190163889602e-003, -1.3278941979339359e-004,};
		static const double HBKernel_9A[ 9 ] = { // -136.6982 dB, 4
			6.2597763804021977e-001, -1.8216414325139055e-001,
			8.2879104876726728e-002, -3.8563442248249404e-002,
			1.6471530499739394e-002, -6.0489108881335227e-003,
			1.7805283804140392e-003, -3.7533200112729561e-004,
			4.3172840558735476e-005,};
		static const double HBKernel_10A[ 10 ] = { // -152.3572 dB, 4
			6.2688767582974092e-001, -1.8460766807559420e-001,
			8.6128943000481864e-002, -4.1774474147006607e-002,
			1.9014801985747346e-002, -7.6870397465866507e-003,
			2.6264590175341853e-003, -7.1106660285478562e-004,
			1.3645852036179345e-004, -1.4113888783332969e-005,};
		static const double HBKernel_11A[ 11 ] = { // -183.7962 dB, 4
			6.2667167706948146e-001, -1.8407153342635879e-001,
			8.5529995610836046e-002, -4.1346831462361310e-002,
			1.8844831691322637e-002, -7.7125170365394992e-003,
			2.7268674860562087e-003, -7.9745028501057233e-004,
			1.8116344606360795e-004, -2.8569149754241848e-005,
			2.3667022010173616e-006,};
		static const double HBKernel_12A[ 12 ] = { // -199.4768 dB, 4
			6.2747849730367999e-001, -1.8623616784506747e-001,
			8.8409755898467945e-002, -4.4207468821462342e-002,
			2.1149175945115381e-002, -9.2551508371115209e-003,
			3.5871562170822330e-003, -1.1923167653750219e-003,
			3.2627812189920129e-004, -6.9106902511490413e-005,
			1.0122897863125124e-005, -7.7531878906846174e-007,};
		static const double HBKernel_13A[ 13 ] = { // -215.1364 dB, 4
			6.2816416252367324e-001, -1.8809076955230414e-001,
			9.0918539867353029e-002, -4.6765502683599310e-002,
			2.3287520498995663e-002, -1.0760627245014184e-002,
			4.4853922948425683e-003, -1.6438775426910800e-003,
			5.1441312354764978e-004, -1.3211725685765050e-004,
			2.6191319837779187e-005, -3.5802430606313093e-006,
			2.5491278270628601e-007,};
		static const double HBKernel_14A[ 14 ] = { // -230.7526 dB, 4
			6.2875473120929948e-001, -1.8969941936903847e-001,
			9.3126094480960403e-002, -4.9067251179869126e-002,
			2.5273008851199916e-002, -1.2218646153393291e-002,
			5.4048942085580280e-003, -2.1409919546078581e-003,
			7.4250292812927973e-004, -2.1924542206832172e-004,
			5.3015808983125091e-005, -9.8743034923598196e-006,
			1.2650391141650221e-006, -8.4146674637474946e-008,};
		static const int FltCountA = 11;
		static const int FlttBaseA = 4;
		static const double FltAttensA[ FltCountA ] = {
			54.5176, 66.3075, 89.5271, 105.2842, 121.0063, 136.6982, 152.3572, 183.7962, 199.4768, 215.1364, 230.7526, };
		static const double* const FltPtrsA[ FltCountA ] = {
			HBKernel_4A, HBKernel_5A, HBKernel_6A, HBKernel_7A, HBKernel_8A, HBKernel_9A, HBKernel_10A, HBKernel_11A, HBKernel_12A, HBKernel_13A, HBKernel_14A, };
		static const double HBKernel_2B[ 2 ] = { // -56.6007 dB, 8
			5.7361525854329076e-001, -7.5092074924827903e-002,};
		static const double HBKernel_3B[ 3 ] = { // -83.0295 dB, 8
			5.9277038608066912e-001, -1.0851340190268854e-001,
			1.5813570475513079e-002,};
		static const double HBKernel_4B[ 4 ] = { // -123.4724 dB, 8
			6.0140277542879617e-001, -1.2564483854574138e-001,
			2.7446500598038322e-002, -3.2051079559057435e-003,};
		static const double HBKernel_5B[ 5 ] = { // -152.4411 dB, 8
			6.0818642429088932e-001, -1.3981140187175697e-001,
			3.8489164054503623e-002, -7.6218861797853104e-003,
			7.5772358130952392e-004,};
		static const double HBKernel_6B[ 6 ] = { // -181.2501 dB, 8
			6.1278392271464355e-001, -1.5000053762513338e-001,
			4.7575323511364960e-002, -1.2320702802243476e-002,
			2.1462442592348487e-003, -1.8425092381892940e-004,};
		static const double HBKernel_7B[ 7 ] = { // -209.9472 dB, 8
			6.1610372263478952e-001, -1.5767891882524138e-001,
			5.5089691170294691e-002, -1.6895755656366061e-002,
			3.9416643438213977e-003, -6.0603623791604668e-004,
			4.5632602433393365e-005,};
		static const double HBKernel_8B[ 8 ] = { // -238.5616 dB, 8
			6.1861282914465976e-001, -1.6367179451225150e-001,
			6.1369861342939716e-002, -2.1184466539006987e-002,
			5.9623357510842061e-003, -1.2483098507454090e-003,
			1.7099297537964702e-004, -1.1448313239478885e-005,};
		static const int FltCountB = 7;
		static const int FlttBaseB = 2;
		static const double FltAttensB[ FltCountB ] = {
			56.6007, 83.0295, 123.4724, 152.4411, 181.2501, 209.9472, 238.5616, };
		static const double* const FltPtrsB[ FltCountB ] = {
			HBKernel_2B, HBKernel_3B, HBKernel_4B, HBKernel_5B, HBKernel_6B, HBKernel_7B, HBKernel_8B, };
		static const double HBKernel_2C[ 2 ] = { // -89.0473 dB, 16
			5.6430278013478008e-001, -6.4338068855763375e-002,};
		static const double HBKernel_3C[ 3 ] = { // -130.8951 dB, 16
			5.8706402915551448e-001, -9.9362380958670449e-002,
			1.2298637065869358e-002,};
		static const double HBKernel_4C[ 4 ] = { // -172.3192 dB, 16
			5.9896586134984675e-001, -1.2111680603434927e-001,
			2.4763118076458895e-002, -2.6121758132212989e-003,};
		static const double HBKernel_5C[ 5 ] = { // -213.4984 dB, 16
			6.0626808285230716e-001, -1.3588224032740795e-001,
			3.5544305238309003e-002, -6.5127022377289654e-003,
			5.8255449565950768e-004,};
		static const double HBKernel_6C[ 6 ] = { // -254.5186 dB, 16
			6.1120171263351242e-001, -1.4654486853757870e-001,
			4.4582959299131253e-002, -1.0840543858123995e-002,
			1.7343706485509962e-003, -1.3363018567985596e-004,};
		static const int FltCountC = 5;
		static const int FlttBaseC = 2;
		static const double FltAttensC[ FltCountC ] = {
			89.0473, 130.8951, 172.3192, 213.4984, 254.5186, };
		static const double* const FltPtrsC[ FltCountC ] = {
			HBKernel_2C, HBKernel_3C, HBKernel_4C, HBKernel_5C, HBKernel_6C, };
		static const double HBKernel_1D[ 1 ] = { // -54.4754 dB, 32
			5.0188900022775451e-001,};
		static const double HBKernel_2D[ 2 ] = { // -113.2139 dB, 32
			5.6295152180538044e-001, -6.2953706070191726e-002,};
		static const double HBKernel_3D[ 3 ] = { // -167.1447 dB, 32
			5.8621968728755036e-001, -9.8080551656524531e-002,
			1.1860868761997080e-002,};
		static const double HBKernel_4D[ 4 ] = { // -220.6519 dB, 32
			5.9835028657163591e-001, -1.1999986086623511e-001,
			2.4132530854004228e-002, -2.4829565686819706e-003,};
		static const int FltCountD = 4;
		static const int FlttBaseD = 1;
		static const double FltAttensD[ FltCountD ] = {
			54.4754, 113.2139, 167.1447, 220.6519, };
		static const double* const FltPtrsD[ FltCountD ] = {
			HBKernel_1D, HBKernel_2D, HBKernel_3D, HBKernel_4D, };
		static const double HBKernel_1E[ 1 ] = { // -66.5391 dB, 64
			5.0047102586416625e-001,};
		static const double HBKernel_2E[ 2 ] = { // -137.3173 dB, 64
			5.6261293163933568e-001, -6.2613067826620017e-002,};
		static const double HBKernel_3E[ 3 ] = { // -203.2997 dB, 64
			5.8600808139396787e-001, -9.7762185880067784e-002,
			1.1754104554493029e-002,};
		static const double HBKernel_4E[ 4 ] = { // -268.8550 dB, 64
			5.9819599352772002e-001, -1.1972157555011861e-001,
			2.3977305567947922e-002, -2.4517235455853992e-003,};
		static const int FltCountE = 4;
		static const int FlttBaseE = 1;
		static const double FltAttensE[ FltCountE ] = {
			66.5391, 137.3173, 203.2997, 268.8550, };
		static const double* const FltPtrsE[ FltCountE ] = {
			HBKernel_1E, HBKernel_2E, HBKernel_3E, HBKernel_4E, };
		static const double HBKernel_1F[ 1 ] = { // -82.4633 dB, 128
			5.0007530666642896e-001,};
		static const double HBKernel_2F[ 2 ] = { // -161.4049 dB, 128
			5.6252823610146030e-001, -6.2528244608044792e-002,};
		static const double HBKernel_3F[ 3 ] = { // -239.4313 dB, 128
			5.8595514744674237e-001, -9.7682725156791952e-002,
			1.1727577711117231e-002,};
		static const int FltCountF = 3;
		static const int FlttBaseF = 1;
		static const double FltAttensF[ FltCountF ] = {
			82.4633, 161.4049, 239.4313, };
		static const double* const FltPtrsF[ FltCountF ] = {
			HBKernel_1F, HBKernel_2F, HBKernel_3F, };
		static const double HBKernel_1G[ 1 ] = { // -94.5052 dB, 256
			5.0001882524896712e-001,};
		static const double HBKernel_2G[ 2 ] = { // -185.4886 dB, 256
			5.6250705922479682e-001, -6.2507059756378394e-002,};
		static const double HBKernel_3G[ 3 ] = { // -275.5501 dB, 256
			5.8594191201187384e-001, -9.7662868266991207e-002,
			1.1720956255134043e-002,};
		static const int FltCountG = 3;
		static const int FlttBaseG = 1;
		static const double FltAttensG[ FltCountG ] = {
			94.5052, 185.4886, 275.5501, };
		static const double* const FltPtrsG[ FltCountG ] = {
			HBKernel_1G, HBKernel_2G, HBKernel_3G, };

		int k = 0;

		if( SteepIndex <= 0 )
		{
			while( k != FltCountA - 1 && FltAttensA[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsA[ k ];
			fltt = FlttBaseA + k;
			att = FltAttensA[ k ];
		}
		else
		if( SteepIndex == 1 )
		{
			while( k != FltCountB - 1 && FltAttensB[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsB[ k ];
			fltt = FlttBaseB + k;
			att = FltAttensB[ k ];
		}
		else
		if( SteepIndex == 2 )
		{
			while( k != FltCountC - 1 && FltAttensC[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsC[ k ];
			fltt = FlttBaseC + k;
			att = FltAttensC[ k ];
		}
		else
		if( SteepIndex == 3 )
		{
			while( k != FltCountD - 1 && FltAttensD[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsD[ k ];
			fltt = FlttBaseD + k;
			att = FltAttensD[ k ];
		}
		else
		if( SteepIndex == 4 )
		{
			while( k != FltCountE - 1 && FltAttensE[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsE[ k ];
			fltt = FlttBaseE + k;
			att = FltAttensE[ k ];
		}
		else
		if( SteepIndex == 5 )
		{
			while( k != FltCountF - 1 && FltAttensF[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsF[ k ];
			fltt = FlttBaseF + k;
			att = FltAttensF[ k ];
		}
		else
		{
			while( k != FltCountG - 1 && FltAttensG[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsG[ k ];
			fltt = FlttBaseG + k;
			att = FltAttensG[ k ];
		}
	}

	/**
	 * Function that provides filter data for various steepness indices and
	 * attenuations. For 1/3 resamplings.
	 *
	 * @param ReqAtten Required half-band filter attentuation.
	 * @param SteepIndex Steepness index - 0=steepest. Corresponds to general
	 * upsampling/downsampling ratio, e.g. at 4x 0 is used, at 8x 1 is used,
	 * etc.
	 */

	static void getHBFilterThird( const double ReqAtten, const int SteepIndex,
		const double*& flt, int& fltt, double& att )
	{
		static const double HBKernel_3A[ 3 ] = { // -66.3726 dB, 6
			5.9811355069551475e-001, -1.1793396656733847e-001,
			2.0300557211946322e-002,};
		static const double HBKernel_4A[ 4 ] = { // -90.2546 dB, 6
			6.0645499250612578e-001, -1.3555496505481171e-001,
			3.4022804962365975e-002, -4.9535418595798757e-003,};
		static const double HBKernel_5A[ 5 ] = { // -126.5507 dB, 6
			6.1014115058940210e-001, -1.4393081816629907e-001,
			4.1760642892852244e-002, -8.9692183234056175e-003,
			9.9871340618342070e-004,};
		static const double HBKernel_6A[ 6 ] = { // -150.1839 dB, 6
			6.1439563420546972e-001, -1.5360187826905250e-001,
			5.0840891345687034e-002, -1.4053648740561121e-002,
			2.6771286587305727e-003, -2.5815816044823123e-004,};
		static const double HBKernel_7A[ 7 ] = { // -173.7068 dB, 6
			6.1747493476329918e-001, -1.6087373733313212e-001,
			5.8263075641409430e-002, -1.8872408173431318e-002,
			4.7421376543513687e-003, -8.0196529612267474e-004,
			6.7964807393798996e-005,};
		static const double HBKernel_8A[ 8 ] = { // -197.1454 dB, 6
			6.1980610947775050e-001, -1.6654070578314714e-001,
			6.4416567441730327e-002, -2.3307744348719822e-002,
			6.9909157372312443e-003, -1.5871946293364403e-003,
			2.4017727382382763e-004, -1.8125308241541697e-005,};
		static const double HBKernel_9A[ 9 ] = { // -220.5199 dB, 6
			6.2163188951899306e-001, -1.7108115323810941e-001,
			6.9588370095600260e-002, -2.7339625080613838e-002,
			9.2954469183791771e-003, -2.5537179959555429e-003,
			5.2572290897951021e-004, -7.1813356135154921e-005,
			4.8802382808892154e-006,};
		static const int FltCountA = 7;
		static const int FlttBaseA = 3;
		static const double FltAttensA[ FltCountA ] = {
			66.3726, 90.2546, 126.5507, 150.1839, 173.7068, 197.1454, 220.5199, };
		static const double* const FltPtrsA[ FltCountA ] = {
			HBKernel_3A, HBKernel_4A, HBKernel_5A, HBKernel_6A, HBKernel_7A, HBKernel_8A, HBKernel_9A, };
		static const double HBKernel_2B[ 2 ] = { // -71.0965 dB, 12
			5.6748544264806311e-001, -6.7764090509431732e-002,};
		static const double HBKernel_3B[ 3 ] = { // -115.7707 dB, 12
			5.8793612182667199e-001, -1.0070583248877293e-001,
			1.2771337947163834e-002,};
		static const double HBKernel_4B[ 4 ] = { // -152.1535 dB, 12
			5.9960155600862808e-001, -1.2228154335199336e-001,
			2.5433718917694709e-002, -2.7537562530837154e-003,};
		static const double HBKernel_5B[ 5 ] = { // -188.2914 dB, 12
			6.0676859170554343e-001, -1.3689667009876413e-001,
			3.6288512631926818e-002, -6.7838855305035351e-003,
			6.2345167677087547e-004,};
		static const double HBKernel_6B[ 6 ] = { // -224.2705 dB, 12
			6.1161456341904397e-001, -1.4743901958274458e-001,
			4.5344160157313275e-002, -1.1207371780924531e-002,
			1.8328497112594935e-003, -1.4518193006359589e-004,};
		static const int FltCountB = 5;
		static const int FlttBaseB = 2;
		static const double FltAttensB[ FltCountB ] = {
			71.0965, 115.7707, 152.1535, 188.2914, 224.2705, };
		static const double* const FltPtrsB[ FltCountB ] = {
			HBKernel_2B, HBKernel_3B, HBKernel_4B, HBKernel_5B, HBKernel_6B, };
		static const double HBKernel_1C[ 1 ] = { // -49.4544 dB, 24
			5.0336730531430562e-001,};
		static const double HBKernel_2C[ 2 ] = { // -103.1970 dB, 24
			5.6330232648142819e-001, -6.3309247177420452e-002,};
		static const double HBKernel_3C[ 3 ] = { // -152.1195 dB, 24
			5.8643891113580415e-001, -9.8411593011583087e-002,
			1.1972706651483846e-002,};
		static const double HBKernel_4C[ 4 ] = { // -200.6182 dB, 24
			5.9851012363917222e-001, -1.2028885239978220e-001,
			2.4294521083140615e-002, -2.5157924156609776e-003,};
		static const double HBKernel_5C[ 5 ] = { // -248.8730 dB, 24
			6.0590922882030196e-001, -1.3515953438018685e-001,
			3.5020857107815606e-002, -6.3256196990467053e-003,
			5.5506815147598793e-004,};
		static const int FltCountC = 5;
		static const int FlttBaseC = 1;
		static const double FltAttensC[ FltCountC ] = {
			49.4544, 103.1970, 152.1195, 200.6182, 248.8730, };
		static const double* const FltPtrsC[ FltCountC ] = {
			HBKernel_1C, HBKernel_2C, HBKernel_3C, HBKernel_4C, HBKernel_5C, };
		static const double HBKernel_1D[ 1 ] = { // -61.5357 dB, 48
			5.0083794231068057e-001,};
		static const double HBKernel_2D[ 2 ] = { // -127.3167 dB, 48
			5.6270074379958690e-001, -6.2701174487726344e-002,};
		static const double HBKernel_3D[ 3 ] = { // -188.2990 dB, 48
			5.8606296210323228e-001, -9.7844644765123029e-002,
			1.1781683046528768e-002,};
		static const double HBKernel_4D[ 4 ] = { // -248.8580 dB, 48
			5.9823601243162516e-001, -1.1979368994739022e-001,
			2.4017458606412575e-002, -2.4597810910081913e-003,};
		static const int FltCountD = 4;
		static const int FlttBaseD = 1;
		static const double FltAttensD[ FltCountD ] = {
			61.5357, 127.3167, 188.2990, 248.8580, };
		static const double* const FltPtrsD[ FltCountD ] = {
			HBKernel_1D, HBKernel_2D, HBKernel_3D, HBKernel_4D, };
		static const double HBKernel_1E[ 1 ] = { // -77.4651 dB, 96
			5.0013388897382527e-001,};
		static const double HBKernel_2E[ 2 ] = { // -151.4084 dB, 96
			5.6255019604317880e-001, -6.2550222932381064e-002,};
		static const double HBKernel_3E[ 3 ] = { // -224.4365 dB, 96
			5.8596887234201078e-001, -9.7703321113080305e-002,
			1.1734448777069783e-002,};
		static const int FltCountE = 3;
		static const int FlttBaseE = 1;
		static const double FltAttensE[ FltCountE ] = {
			77.4651, 151.4084, 224.4365, };
		static const double* const FltPtrsE[ FltCountE ] = {
			HBKernel_1E, HBKernel_2E, HBKernel_3E, };
		static const double HBKernel_1F[ 1 ] = { // -89.5075 dB, 192
			5.0003346776264190e-001,};
		static const double HBKernel_2F[ 2 ] = { // -175.4932 dB, 192
			5.6251254964097952e-001, -6.2512551321105267e-002,};
		static const double HBKernel_3F[ 3 ] = { // -260.5645 dB, 192
			5.8594534336747051e-001, -9.7668015838639821e-002,
			1.1722672471262996e-002,};
		static const int FltCountF = 3;
		static const int FlttBaseF = 1;
		static const double FltAttensF[ FltCountF ] = {
			89.5075, 175.4932, 260.5645, };
		static const double* const FltPtrsF[ FltCountF ] = {
			HBKernel_1F, HBKernel_2F, HBKernel_3F, };
		static const double HBKernel_1G[ 1 ] = { // -101.5490 dB, 384
			5.0000836666064941e-001,};
		static const double HBKernel_2G[ 2 ] = { // -199.5761 dB, 384
			5.6250313744943459e-001, -6.2503137554435345e-002,};
		static const double HBKernel_3G[ 3 ] = { // -296.5185 dB, 384
			5.8593945786963764e-001, -9.7659186853499613e-002,
			1.1719728983863425e-002,};
		static const int FltCountG = 3;
		static const int FlttBaseG = 1;
		static const double FltAttensG[ FltCountG ] = {
			101.5490, 199.5761, 296.5185, };
		static const double* const FltPtrsG[ FltCountG ] = {
			HBKernel_1G, HBKernel_2G, HBKernel_3G, };

		int k = 0;

		if( SteepIndex <= 0 )
		{
			while( k != FltCountA - 1 && FltAttensA[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsA[ k ];
			fltt = FlttBaseA + k;
			att = FltAttensA[ k ];
		}
		else
		if( SteepIndex == 1 )
		{
			while( k != FltCountB - 1 && FltAttensB[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsB[ k ];
			fltt = FlttBaseB + k;
			att = FltAttensB[ k ];
		}
		else
		if( SteepIndex == 2 )
		{
			while( k != FltCountC - 1 && FltAttensC[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsC[ k ];
			fltt = FlttBaseC + k;
			att = FltAttensC[ k ];
		}
		else
		if( SteepIndex == 3 )
		{
			while( k != FltCountD - 1 && FltAttensD[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsD[ k ];
			fltt = FlttBaseD + k;
			att = FltAttensD[ k ];
		}
		else
		if( SteepIndex == 4 )
		{
			while( k != FltCountE - 1 && FltAttensE[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsE[ k ];
			fltt = FlttBaseE + k;
			att = FltAttensE[ k ];
		}
		else
		if( SteepIndex == 5 )
		{
			while( k != FltCountF - 1 && FltAttensF[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsF[ k ];
			fltt = FlttBaseF + k;
			att = FltAttensF[ k ];
		}
		else
		{
			while( k != FltCountG - 1 && FltAttensG[ k ] < ReqAtten )
			{
				k++;
			}

			flt = FltPtrsG[ k ];
			fltt = FlttBaseG + k;
			att = FltAttensG[ k ];
		}
	}

	/**
	 * Constructor initalizes the half-band upsampler.
	 *
	 * @param ReqAtten Required half-band filter attentuation.
	 * @param SteepIndex Steepness index - 0=steepest. Corresponds to general
	 * upsampling ratio, e.g. at 4x upsampling 0 is used, at 8x upsampling 1
	 * is used, etc.
	 * @param IsThird "True" if 1/3 of frequency response resampling is
	 * performed.
	 * @param PrevLatency Latency, in samples (any value >=0), which was left
	 * in the output signal by a previous process. Whole-number latency will
	 * be consumed by *this object while remaining fractional latency can be
	 * obtained via the getLatencyFrac() function.
	 * @param aDoConsumeLatency "True" if the output latency should be
	 * consumed. Does not apply to the fractional part of the latency (if such
	 * part is available).
	 */

	CDSPHBUpsampler( const double ReqAtten, const int SteepIndex,
		const bool IsThird, const double PrevLatency,
		const bool aDoConsumeLatency = true )
		: DoConsumeLatency( aDoConsumeLatency )
	{
		static const CConvolveFn FltConvFn[ 14 ] = {
			&CDSPHBUpsampler :: convolve1, &CDSPHBUpsampler :: convolve2,
			&CDSPHBUpsampler :: convolve3, &CDSPHBUpsampler :: convolve4,
			&CDSPHBUpsampler :: convolve5, &CDSPHBUpsampler :: convolve6,
			&CDSPHBUpsampler :: convolve7, &CDSPHBUpsampler :: convolve8,
			&CDSPHBUpsampler :: convolve9, &CDSPHBUpsampler :: convolve10,
			&CDSPHBUpsampler :: convolve11, &CDSPHBUpsampler :: convolve12,
			&CDSPHBUpsampler :: convolve13, &CDSPHBUpsampler :: convolve14 };

		const double* fltp0;
		int fltt;
		double att;

		if( IsThird )
		{
			getHBFilterThird( ReqAtten, SteepIndex, fltp0, fltt, att );
		}
		else
		{
			getHBFilter( ReqAtten, SteepIndex, fltp0, fltt, att );
		}

		// Copy obtained filter to address-aligned buffer.

		fltp = alignptr( FltBuf, 16 );
		memcpy( fltp, fltp0, fltt * sizeof( fltp[ 0 ]));

		convfn = FltConvFn[ fltt - 1 ];
		fll = fltt - 1;
		fl2 = fltt;
		flo = fll + fl2;
		BufRP = Buf + fll;

		LatencyFrac = PrevLatency * 2.0;
		Latency = (int) LatencyFrac;
		LatencyFrac -= Latency;

		R8BASSERT( Latency >= 0 );

		if( DoConsumeLatency )
		{
			flb = BufLen - fll;
		}
		else
		{
			Latency += fl2 + fl2;
			flb = BufLen - flo;
		}

		R8BCONSOLE( "CDSPHBUpsampler: sti=%i third=%i taps=%i att=%.1f "
			"io=2/1\n", SteepIndex, (int) IsThird, fltt, att );

		clear();
	}

	virtual int getLatency() const
	{
		return( DoConsumeLatency ? 0 : Latency );
	}

	virtual double getLatencyFrac() const
	{
		return( LatencyFrac );
	}

	virtual int getMaxOutLen( const int MaxInLen ) const
	{
		R8BASSERT( MaxInLen >= 0 );

		return( MaxInLen << 1 );
	}

	virtual void clear()
	{
		if( DoConsumeLatency )
		{
			LatencyLeft = Latency;
			BufLeft = 0;
		}
		else
		{
			LatencyLeft = 0;
			BufLeft = fl2;
		}

		WritePos = 0;
		ReadPos = flb; // Set "read" position to account for filter's latency.

		memset( &Buf[ ReadPos ], 0, ( BufLen - flb ) * sizeof( Buf[ 0 ]));
	}

	virtual int process( double* ip, int l, double*& op0 )
	{
		R8BASSERT( l >= 0 );

		double* op = op0;

		while( l > 0 )
		{
			// Add new input samples to both halves of the ring buffer.

			const int b = min( min( l, BufLen - WritePos ), flb - BufLeft );

			double* const wp1 = Buf + WritePos;
			memcpy( wp1, ip, b * sizeof( wp1[ 0 ]));

			if( WritePos < flo )
			{
				const int c = min( b, flo - WritePos );
				memcpy( wp1 + BufLen, wp1, c * sizeof( wp1[ 0 ]));
			}

			ip += b;
			WritePos = ( WritePos + b ) & BufLenMask;
			l -= b;
			BufLeft += b;

			// Produce output.

			if( BufLeft > fl2 )
			{
				const int c = BufLeft - fl2;

				double* const opend = op + ( c + c );
				( *convfn )( op, opend, fltp, BufRP, ReadPos );

				op = opend;
				ReadPos = ( ReadPos + c ) & BufLenMask;
				BufLeft -= c;
			}
		}

		int ol = (int) ( op - op0 );

		if( LatencyLeft != 0 )
		{
			if( LatencyLeft >= ol )
			{
				LatencyLeft -= ol;
				return( 0 );
			}

			ol -= LatencyLeft;
			op0 += LatencyLeft;
			LatencyLeft = 0;
		}

		return( ol );
	}

private:
	static const int BufLenBits = 9; ///< The length of the ring buffer,
		///< expressed as Nth power of 2. This value can be reduced if it is
		///< known that only short input buffers will be passed to the
		///< interpolator. The minimum value of this parameter is 5, and
		///< 1 << BufLenBits should be at least 3 times larger than the
		///< FilterLen.
		///<
	static const int BufLen = 1 << BufLenBits; ///< The length of the ring
		///< buffer. The actual length is twice as long to allow "beyond max
		///< position" positioning.
		///<
	static const int BufLenMask = BufLen - 1; ///< Mask used for quick buffer
		///< position wrapping.
		///<
	double Buf[ BufLen + 27 ]; ///< The ring buffer, including overrun
		///< protection for the largest filter.
		///<
	double FltBuf[ 14 + 2 ]; ///< Holder for half-band filter taps, used with
		///< 16-byte address-aligning, for SIMD use.
		///<
	double* fltp; ///< Half-band filter taps, points to FltBuf.
		///<
	int fll; ///< Input latency.
		///<
	int fl2; ///< Right-side filter length.
		///<
	int flo; ///< Overrrun length.
		///<
	int flb; ///< Initial read position and maximal buffer write length.
		///<
	const double* BufRP; ///< Offseted Buf pointer at ReadPos=0.
		///<
	bool DoConsumeLatency; ///< "True" if the output latency should be
		///< consumed. Does not apply to the fractional part of the latency
		///< (if such part is available).
		///<
	int Latency; ///< Initial latency that should be removed from the output.
		///<
	double LatencyFrac; ///< Fractional latency left on the output.
		///<
	int BufLeft; ///< The number of samples left in the buffer to process.
		///< When this value is below FilterLenD2Plus1, the interpolation
		///< cycle ends.
		///<
	int WritePos; ///< The current buffer write position. Incremented together
		///< with the BufLeft variable.
		///<
	int ReadPos; ///< The current buffer read position.
		///<
	int LatencyLeft; ///< Latency left to remove.
		///<
	typedef void( *CConvolveFn )( double* op, double* const opend,
		const double* const flt, const double* const rp0, int rpos ); ///<
		///< Convolution function type.
		///<
	CConvolveFn convfn; ///< Convolution function in use.
		///<

#define R8BHBC1( fn ) \
	static void fn( double* op, double* const opend, const double* const flt, \
		const double* const rp0, int rpos ) \
	{ \
		while( op != opend ) \
		{ \
			const double* const rp = rp0 + rpos; \
			op[ 0 ] = rp[ 0 ];

#define R8BHBC2 \
			rpos = ( rpos + 1 ) & BufLenMask; \
			op += 2; \
		} \
	}

#include "CDSPHBUpsampler.inc"

#undef R8BHBC1
#undef R8BHBC2
};

// ---------------------------------------------------------------------------

} // namespace r8b

#endif // R8B_CDSPHBUPSAMPLER_INCLUDED
