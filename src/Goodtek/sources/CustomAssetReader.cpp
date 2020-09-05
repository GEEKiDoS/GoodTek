#include "common_includes.hpp"

#include "IW5.hpp"
#include "CustomAssetReader.hpp"

using namespace IW5;

#define WEAPON_READ_FIELD(__type__, __field__) \
	if (!data[#__field__].is_null()) weapon->__field__ = data[#__field__].get<__type__>()

#define WEAPON_READ_STRING(__field__) \
	if (!data[#__field__].is_null()) weapon->__field__ = mem->StrDup(data[#__field__].get<std::string>())

#define WEAPON_READ_ASSET(__type__, __datafield__, __field__) \
	if (!data[#__field__].is_null() && data[#__field__].is_string()) \
	{ \
		auto asset##__field__ = data[#__field__].get<std::string>(); \
		if (asset##__field__.empty()) \
		{ \
			weapon->__field__ = nullptr; \
		} \
		else \
		{ \
			weapon->__field__ = IW5::DB_FindXAssetHeader(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
		} \
	}


#pragma optimize("", off)
WeaponDef* ReadWeaponDef(Json & data, WeaponCompleteDef * baseAsset, ZoneMemory * mem)
{
	auto weapon = mem->Alloc<WeaponDef>();

	if (baseAsset)
	{
		memcpy(weapon, baseAsset->WeaponDef, sizeof WeaponDef);
	}

	WEAPON_READ_STRING(szInternalName);

	weapon->gunXModel = mem->Alloc<XModel*>(16);
	weapon->worldModel = mem->Alloc<XModel*>(16);

	for (int i = 0; i < 16; i++)
	{
		auto gunmodel = data["gunXModel"][i].get<std::string>();

		if (!gunmodel.empty())
		{
			weapon->gunXModel[i] = IW5::DB_FindXAssetHeader(xmodel, gunmodel.data(), 1).xmodel;
		}
	}
	for (int i = 0; i < 16; i++)
	{
		auto gunmodel = data["worldXModel"][i].get<std::string>();

		if (!gunmodel.empty())
		{
			weapon->worldModel[i] = IW5::DB_FindXAssetHeader(xmodel, gunmodel.data(), 1).xmodel;
		}
	}

	WEAPON_READ_ASSET(xmodel, xmodel, handXModel);

	weapon->notetrackSoundMapKeys = mem->Alloc<short>(24);
	weapon->notetrackSoundMapValues = mem->Alloc<short>(24);
	for (int i = 0; i < 24; i++)
	{
		auto notetrack = data["notetrackSoundMapKeys"][i].get<std::string>();
		weapon->notetrackSoundMapKeys[i] = SL_AllocString(notetrack);
		printf("notetrack %s %i\n", notetrack.data(), weapon->notetrackSoundMapKeys[i]);
	}
	for (int i = 0; i < 24; i++)
	{
		auto notetrack = data["notetrackSoundMapValues"][i].get<std::string>();
		weapon->notetrackSoundMapValues[i] = SL_AllocString(notetrack);
		printf("notetrack %s %i\n", notetrack.data(), weapon->notetrackSoundMapValues[i]);
	}

	weapon->notetrackRumbleMapKeys = mem->Alloc<short>(16);
	weapon->notetrackRumbleMapValues = mem->Alloc<short>(16);
	for (int i = 0; i < 16; i++)
	{
		auto notetrack = data["notetrackRumbleMapKeys"][i].get<std::string>();
		weapon->notetrackRumbleMapKeys[i] = SL_AllocString(notetrack);
		printf("notetrack %s %i\n", notetrack.data(), weapon->notetrackRumbleMapKeys[i]);
	}
	for (int i = 0; i < 16; i++)
	{
		auto notetrack = data["notetrackRumbleMapValues"][i].get<std::string>();
		weapon->notetrackRumbleMapValues[i] = SL_AllocString(notetrack);
		printf("notetrack %s %i\n", notetrack.data(), weapon->notetrackRumbleMapValues[i]);
	}

	WEAPON_READ_FIELD(int, playerAnimType);
	WEAPON_READ_FIELD(int, weaponType);
	WEAPON_READ_FIELD(int, weaponClass);
	WEAPON_READ_FIELD(int, penetrateType);
	WEAPON_READ_FIELD(int, inventoryType);
	WEAPON_READ_FIELD(int, fireType);
	WEAPON_READ_FIELD(int, offhandClass);
	WEAPON_READ_FIELD(int, stance);
	WEAPON_READ_ASSET(fx, fx, viewFlashEffect);
	WEAPON_READ_ASSET(fx, fx, worldFlashEffect);
	WEAPON_READ_ASSET(fx, fx, viewShellEjectEffect);
	WEAPON_READ_ASSET(fx, fx, worldShellEjectEffect);
	WEAPON_READ_ASSET(fx, fx, viewLastShotEjectEffect);
	WEAPON_READ_ASSET(fx, fx, worldLastShotEjectEffect);
	WEAPON_READ_ASSET(material, material, reticleCenter);
	WEAPON_READ_ASSET(material, material, reticleSide);

	for (int i = 0; i < 48; i++)
	{
		auto sound = data["sounds"][i].get<std::string>();

		if (!sound.empty())
		{
			weapon->sounds[i] = IW5::DB_FindXAssetHeader(XAssetType::sound, sound.data(), 1).sound;
		}
	}

	WEAPON_READ_FIELD(int, iReticleCenterSize);
	WEAPON_READ_FIELD(int, iReticleSideSize);
	WEAPON_READ_FIELD(int, iReticleMinOfs);
	// WEAPON_READ_FIELD(activeReticleType_t, activeReticleType);
	/*WEAPON_READ_FIELD(float, vStandMove[3]);
	WEAPON_READ_FIELD(float, vStandRot[3]);
	WEAPON_READ_FIELD(float, vStrafeMove[3]);
	WEAPON_READ_FIELD(float, vStrafeRot[3]);
	WEAPON_READ_FIELD(float, vDuckedOfs[3]);
	WEAPON_READ_FIELD(float, vDuckedMove[3]);
	WEAPON_READ_FIELD(float, vDuckedRot[3]);
	WEAPON_READ_FIELD(float, vProneOfs[3]);
	WEAPON_READ_FIELD(float, vProneMove[3]);
	WEAPON_READ_FIELD(float, vProneRot[3]);*/
	WEAPON_READ_FIELD(float, fPosMoveRate);
	WEAPON_READ_FIELD(float, fPosProneMoveRate);
	WEAPON_READ_FIELD(float, fStandMoveMinSpeed);
	WEAPON_READ_FIELD(float, fDuckedMoveMinSpeed);
	WEAPON_READ_FIELD(float, fProneMoveMinSpeed);
	WEAPON_READ_FIELD(float, fPosRotRate);
	WEAPON_READ_FIELD(float, fPosProneRotRate);
	WEAPON_READ_FIELD(float, fStandRotMinSpeed);
	WEAPON_READ_FIELD(float, fDuckedRotMinSpeed);
	WEAPON_READ_FIELD(float, fProneRotMinSpeed);
	WEAPON_READ_ASSET(xmodel, xmodel, worldClipModel);
	WEAPON_READ_ASSET(xmodel, xmodel, rocketModel);
	WEAPON_READ_ASSET(xmodel, xmodel, knifeModel);
	WEAPON_READ_ASSET(xmodel, xmodel, worldKnifeModel);
	WEAPON_READ_ASSET(material, material, hudIcon);
	WEAPON_READ_ASSET(material, material, pickupIcon);
	WEAPON_READ_ASSET(material, material, ammoCounterIcon);
	// WEAPON_READ_FIELD(weaponIconRatioType_t, hudIconRatio);
	// WEAPON_READ_FIELD(weaponIconRatioType_t, pickupIconRatio);
	// WEAPON_READ_FIELD(weaponIconRatioType_t, ammoCounterIconRatio);
	// WEAPON_READ_FIELD(ammoCounterClipType_t, ammoCounterClip);
	WEAPON_READ_FIELD(int, iStartAmmo);
	WEAPON_READ_STRING(szAmmoName);
	WEAPON_READ_FIELD(int, iAmmoIndex);
	WEAPON_READ_STRING(szClipName);
	WEAPON_READ_FIELD(int, iClipIndex);
	WEAPON_READ_FIELD(int, iMaxAmmo);
	WEAPON_READ_FIELD(int, shotCount);
	WEAPON_READ_STRING(szSharedAmmoCapName);
	WEAPON_READ_FIELD(int, iSharedAmmoCapIndex);
	WEAPON_READ_FIELD(int, iSharedAmmoCap);
	WEAPON_READ_FIELD(int, damage);
	WEAPON_READ_FIELD(int, playerDamage);
	WEAPON_READ_FIELD(int, iMeleeDamage);
	WEAPON_READ_FIELD(int, iDamageType);
	WEAPON_READ_FIELD(int, iFireDelay);
	WEAPON_READ_FIELD(int, iMeleeDelay);
	WEAPON_READ_FIELD(int, meleeChargeDelay);
	WEAPON_READ_FIELD(int, iDetonateDelay);
	WEAPON_READ_FIELD(int, iRechamberTime);
	WEAPON_READ_FIELD(int, iRechamberOneHanded);
	WEAPON_READ_FIELD(int, iRechamberBoltTime);
	WEAPON_READ_FIELD(int, iHoldFireTime);
	WEAPON_READ_FIELD(int, iDetonateTime);
	WEAPON_READ_FIELD(int, iMeleeTime);
	WEAPON_READ_FIELD(int, meleeChargeTime);
	WEAPON_READ_FIELD(int, iReloadTime);
	WEAPON_READ_FIELD(int, reloadShowRocketTime);
	WEAPON_READ_FIELD(int, iReloadEmptyTime);
	WEAPON_READ_FIELD(int, iReloadAddTime);
	WEAPON_READ_FIELD(int, iReloadStartTime);
	WEAPON_READ_FIELD(int, iReloadStartAddTime);
	WEAPON_READ_FIELD(int, iReloadEndTime);
	WEAPON_READ_FIELD(int, iDropTime);
	WEAPON_READ_FIELD(int, iRaiseTime);
	WEAPON_READ_FIELD(int, iAltDropTime);
	WEAPON_READ_FIELD(int, quickDropTime);
	WEAPON_READ_FIELD(int, quickRaiseTime);
	WEAPON_READ_FIELD(int, iFirstRaiseTime);
	WEAPON_READ_FIELD(int, iEmptyRaiseTime);
	WEAPON_READ_FIELD(int, iEmptyDropTime);
	WEAPON_READ_FIELD(int, sprintInTime);
	WEAPON_READ_FIELD(int, sprintLoopTime);
	WEAPON_READ_FIELD(int, sprintOutTime);
	WEAPON_READ_FIELD(int, stunnedTimeBegin);
	WEAPON_READ_FIELD(int, stunnedTimeLoop);
	WEAPON_READ_FIELD(int, stunnedTimeEnd);
	WEAPON_READ_FIELD(int, nightVisionWearTime);
	WEAPON_READ_FIELD(int, nightVisionWearTimeFadeOutEnd);
	WEAPON_READ_FIELD(int, nightVisionWearTimePowerUp);
	WEAPON_READ_FIELD(int, nightVisionRemoveTime);
	WEAPON_READ_FIELD(int, nightVisionRemoveTimePowerDown);
	WEAPON_READ_FIELD(int, nightVisionRemoveTimeFadeInStart);
	WEAPON_READ_FIELD(int, fuseTime);
	WEAPON_READ_FIELD(int, aifuseTime);
	WEAPON_READ_FIELD(float, autoAimRange);
	WEAPON_READ_FIELD(float, aimAssistRange);
	WEAPON_READ_FIELD(float, aimAssistRangeAds);
	WEAPON_READ_FIELD(float, aimPadding);
	WEAPON_READ_FIELD(float, enemyCrosshairRange);
	WEAPON_READ_FIELD(float, moveSpeedScale);
	WEAPON_READ_FIELD(float, adsMoveSpeedScale);
	WEAPON_READ_FIELD(float, sprintDurationScale);
	WEAPON_READ_FIELD(float, adsZoomInFrac);
	WEAPON_READ_FIELD(float, adsZoomOutFrac);
	WEAPON_READ_ASSET(material, material, AdsOverlayShader);
	WEAPON_READ_ASSET(material, material, AdsOverlayShaderLowRes);
	WEAPON_READ_ASSET(material, material, AdsOverlayShaderEMP);
	WEAPON_READ_ASSET(material, material, AdsOverlayShaderEMPLowRes);
	// WEAPON_READ_FIELD(weapOverlayReticle_t, adsOverlayReticle);
	// WEAPON_READ_FIELD(weapOverlayInterface_t, adsOverlayInterface);
	WEAPON_READ_FIELD(float, adsOverlayWidth);
	WEAPON_READ_FIELD(float, adsOverlayHeight);
	WEAPON_READ_FIELD(float, adsOverlayWidthSplitscreen);
	WEAPON_READ_FIELD(float, adsOverlayHeightSplitscreen);
	WEAPON_READ_FIELD(float, fAdsBobFactor);
	WEAPON_READ_FIELD(float, fAdsViewBobMult);
	WEAPON_READ_FIELD(float, fHipSpreadStandMin);
	WEAPON_READ_FIELD(float, fHipSpreadDuckedMin);
	WEAPON_READ_FIELD(float, fHipSpreadProneMin);
	WEAPON_READ_FIELD(float, hipSpreadStandMax);
	WEAPON_READ_FIELD(float, hipSpreadDuckedMax);
	WEAPON_READ_FIELD(float, hipSpreadProneMax);
	WEAPON_READ_FIELD(float, fHipSpreadDecayRate);
	WEAPON_READ_FIELD(float, fHipSpreadFireAdd);
	WEAPON_READ_FIELD(float, fHipSpreadTurnAdd);
	WEAPON_READ_FIELD(float, fHipSpreadMoveAdd);
	WEAPON_READ_FIELD(float, fHipSpreadDuckedDecay);
	WEAPON_READ_FIELD(float, fHipSpreadProneDecay);
	WEAPON_READ_FIELD(float, fHipReticleSidePos);
	WEAPON_READ_FIELD(float, fAdsIdleAmount);
	WEAPON_READ_FIELD(float, fHipIdleAmount);
	WEAPON_READ_FIELD(float, adsIdleSpeed);
	WEAPON_READ_FIELD(float, hipIdleSpeed);
	WEAPON_READ_FIELD(float, fIdleCrouchFactor);
	WEAPON_READ_FIELD(float, fIdleProneFactor);
	WEAPON_READ_FIELD(float, fGunMaxPitch);
	WEAPON_READ_FIELD(float, fGunMaxYaw);
	WEAPON_READ_FIELD(float, swayMaxAngle);
	WEAPON_READ_FIELD(float, swayLerpSpeed);
	WEAPON_READ_FIELD(float, swayPitchScale);
	WEAPON_READ_FIELD(float, swayYawScale);
	WEAPON_READ_FIELD(float, swayHorizScale);
	WEAPON_READ_FIELD(float, swayVertScale);
	WEAPON_READ_FIELD(float, swayShellShockScale);
	WEAPON_READ_FIELD(float, adsSwayMaxAngle);
	WEAPON_READ_FIELD(float, adsSwayLerpSpeed);
	WEAPON_READ_FIELD(float, adsSwayPitchScale);
	WEAPON_READ_FIELD(float, adsSwayYawScale);
	WEAPON_READ_FIELD(float, adsSwayHorizScale);
	WEAPON_READ_FIELD(float, adsSwayVertScale);
	WEAPON_READ_FIELD(float, adsViewErrorMin);
	WEAPON_READ_FIELD(float, adsViewErrorMax);
	// _BYTE idk[8]);
	// PhysCollmap* collisions);
	WEAPON_READ_ASSET(phys_collmap, physcollmap, collisions);
	WEAPON_READ_FIELD(float, dualWieldViewModelOffset);
	// WEAPON_READ_FIELD(weaponIconRatioType_t, killIconRatio);
	WEAPON_READ_FIELD(int, iReloadAmmoAdd);
	WEAPON_READ_FIELD(int, iReloadStartAdd);
	WEAPON_READ_FIELD(int, iDropAmmoMin);
	WEAPON_READ_FIELD(int, ammoDropClipPercentMin);
	WEAPON_READ_FIELD(int, ammoDropClipPercentMax);
	WEAPON_READ_FIELD(int, iExplosionRadius);
	WEAPON_READ_FIELD(int, iExplosionRadiusMin);
	WEAPON_READ_FIELD(int, iExplosionInnerDamage);
	WEAPON_READ_FIELD(int, iExplosionOuterDamage);
	WEAPON_READ_FIELD(float, damageConeAngle);
	WEAPON_READ_FIELD(float, bulletExplDmgMult);
	WEAPON_READ_FIELD(float, bulletExplRadiusMult);
	WEAPON_READ_FIELD(int, iProjectileSpeed);
	WEAPON_READ_FIELD(int, iProjectileSpeedUp);
	WEAPON_READ_FIELD(int, iProjectileSpeedForward);
	WEAPON_READ_FIELD(int, iProjectileActivateDist);
	WEAPON_READ_FIELD(float, projLifetime);
	WEAPON_READ_FIELD(float, timeToAccelerate);
	WEAPON_READ_FIELD(float, projectileCurvature);
	WEAPON_READ_ASSET(xmodel, xmodel, projectileModel);
	WEAPON_READ_ASSET(fx, fx, projExplosionEffect);
	WEAPON_READ_ASSET(fx, fx, projDudEffect);
	WEAPON_READ_ASSET(sound, sound, projExplosionSound);
	WEAPON_READ_ASSET(sound, sound, projDudSound);
	// WEAPON_READ_FIELD(weapProjExplosion_t, projExplosiveType);
	// WEAPON_READ_FIELD(WeapStickinessType, stickiness);
	WEAPON_READ_FIELD(float, lowAmmoWarningThreshold);
	WEAPON_READ_FIELD(float, ricochetChance);
	// WEAPON_READ_FIELD(float, * parallelBounce);            //Refer to surfaceNames_t
	// WEAPON_READ_FIELD(float, * perpendicularBounce);         //Refer to surfaceNames_t
	// _BYTE gap52D[4]);
	// WEAPON_READ_STRING(unkStruct124_1);
	// WEAPON_READ_STRING(unkStruct124_2);
	WEAPON_READ_ASSET(fx, fx, projTrailEffect);
	WEAPON_READ_ASSET(fx, fx, projBeaconEffect);
	// WEAPON_READ_FIELD(float, vProjectileColor[3]);
	// WEAPON_READ_FIELD(guidedMissileType_t, guidedMissileType);
	WEAPON_READ_FIELD(float, maxSteeringAccel);
	WEAPON_READ_FIELD(float, projIgnitionDelay);
	WEAPON_READ_ASSET(fx, fx, projIgnitionEffect);
	WEAPON_READ_ASSET(sound, sound, projIgnitionSound);
	WEAPON_READ_FIELD(float, fAdsAimPitch);
	WEAPON_READ_FIELD(float, fAdsCrosshairInFrac);
	WEAPON_READ_FIELD(float, fAdsCrosshairOutFrac);
	WEAPON_READ_FIELD(int, adsGunKickReducedKickBullets);
	WEAPON_READ_FIELD(float, adsGunKickReducedKickPercent);
	WEAPON_READ_FIELD(float, fAdsGunKickPitchMin);
	WEAPON_READ_FIELD(float, fAdsGunKickPitchMax);
	WEAPON_READ_FIELD(float, fAdsGunKickYawMin);
	WEAPON_READ_FIELD(float, fAdsGunKickYawMax);
	WEAPON_READ_FIELD(float, fAdsGunKickAccel);
	WEAPON_READ_FIELD(float, fAdsGunKickSpeedMax);
	WEAPON_READ_FIELD(float, fAdsGunKickSpeedDecay);
	WEAPON_READ_FIELD(float, fAdsGunKickStaticDecay);
	WEAPON_READ_FIELD(float, fAdsViewKickPitchMin);
	WEAPON_READ_FIELD(float, fAdsViewKickPitchMax);
	WEAPON_READ_FIELD(float, fAdsViewKickYawMin);
	WEAPON_READ_FIELD(float, fAdsViewKickYawMax);
	WEAPON_READ_FIELD(float, fAdsViewScatterMin);
	WEAPON_READ_FIELD(float, fAdsViewScatterMax);
	WEAPON_READ_FIELD(float, fAdsSpread);
	WEAPON_READ_FIELD(int, hipGunKickReducedKickBullets);
	WEAPON_READ_FIELD(float, hipGunKickReducedKickPercent);
	WEAPON_READ_FIELD(float, fHipGunKickPitchMin);
	WEAPON_READ_FIELD(float, fHipGunKickPitchMax);
	WEAPON_READ_FIELD(float, fHipGunKickYawMin);
	WEAPON_READ_FIELD(float, fHipGunKickYawMax);
	WEAPON_READ_FIELD(float, fHipGunKickAccel);
	WEAPON_READ_FIELD(float, fHipGunKickSpeedMax);
	WEAPON_READ_FIELD(float, fHipGunKickSpeedDecay);
	WEAPON_READ_FIELD(float, fHipGunKickStaticDecay);
	WEAPON_READ_FIELD(float, fHipViewKickPitchMin);
	WEAPON_READ_FIELD(float, fHipViewKickPitchMax);
	WEAPON_READ_FIELD(float, fHipViewKickYawMin);
	WEAPON_READ_FIELD(float, fHipViewKickYawMax);
	WEAPON_READ_FIELD(float, fHipViewScatterMin);
	WEAPON_READ_FIELD(float, fHipViewScatterMax);
	WEAPON_READ_FIELD(float, fightDist);
	WEAPON_READ_FIELD(float, maxDist);
	WEAPON_READ_FIELD(int, iPositionReloadTransTime);
	WEAPON_READ_FIELD(float, leftArc);
	WEAPON_READ_FIELD(float, rightArc);
	WEAPON_READ_FIELD(float, topArc);
	WEAPON_READ_FIELD(float, bottomArc);
	WEAPON_READ_FIELD(float, accuracy);
	WEAPON_READ_FIELD(float, aiSpread);
	WEAPON_READ_FIELD(float, playerSpread);
	WEAPON_READ_FIELD(float, minVertTurnSpeed);
	WEAPON_READ_FIELD(float, minHorTurnSpeed);
	WEAPON_READ_FIELD(float, maxVertTurnSpeed);
	WEAPON_READ_FIELD(float, maxHorTurnSpeed);
	WEAPON_READ_FIELD(float, pitchConvergenceTime);
	WEAPON_READ_FIELD(float, yawConvergenceTime);
	WEAPON_READ_FIELD(float, suppressTime);
	WEAPON_READ_FIELD(float, maxRange);
	WEAPON_READ_FIELD(float, fAnimHorRotateInc);
	WEAPON_READ_FIELD(float, fPlayerPositionDist);
	WEAPON_READ_STRING(szUseHintString);
	WEAPON_READ_STRING(dropHintString);
	WEAPON_READ_FIELD(int, iUseHintStringIndex);
	WEAPON_READ_FIELD(int, dropHintStringIndex);
	WEAPON_READ_FIELD(float, horizViewJitter);
	WEAPON_READ_FIELD(float, vertViewJitter);
	WEAPON_READ_FIELD(float, scanSpeed);
	WEAPON_READ_FIELD(float, scanAccel);
	WEAPON_READ_FIELD(int, scanPauseTime);
	WEAPON_READ_STRING(szScript);
	// WEAPON_READ_FIELD(float, fOOPosAnimLength[2]);
	WEAPON_READ_FIELD(int, minDamage);
	WEAPON_READ_FIELD(int, minPlayerDamage);
	WEAPON_READ_FIELD(float, maxDamageRange);
	WEAPON_READ_FIELD(float, minDamageRange);
	WEAPON_READ_FIELD(float, destabilizationRateTime);
	WEAPON_READ_FIELD(float, destabilizationCurvatureMax);
	WEAPON_READ_FIELD(int, destabilizeDistance);
	// float* locationDamageMultipliers);
	WEAPON_READ_STRING(fireRumble);
	WEAPON_READ_STRING(meleeImpactRumble);
	WEAPON_READ_ASSET(tracer, tracer, tracer);
	WEAPON_READ_ASSET(fx, fx, turretOverheatEffect);
	WEAPON_READ_ASSET(sound, sound, turretOverheatSound);
	WEAPON_READ_STRING(turretBarrelSpinRumble);
	WEAPON_READ_FIELD(int, turretBarrelSpinUpTime);
	WEAPON_READ_FIELD(int, turretBarrelSpinDownTime);
	WEAPON_READ_FIELD(int, turretBarrelSpinSpeed);
	WEAPON_READ_ASSET(sound, sound, turretBarrelSpinMaxSnd);
	WEAPON_READ_ASSET(sound, sound, missileConeSoundAlias);
	WEAPON_READ_ASSET(sound, sound, missileConeSoundAliasAtBase);
	// snd_alias_list_t* turretBarrelSpinUpSnds[4]);
	// snd_alias_list_t* turretBarrelSpinDownSnds[4]);
	WEAPON_READ_FIELD(float, missileConeSoundRadiusAtTop);
	WEAPON_READ_FIELD(float, missileConeSoundRadiusAtBase);
	WEAPON_READ_FIELD(float, missileConeSoundHeight);
	WEAPON_READ_FIELD(float, missileConeSoundOriginOffset);
	WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtCore);
	WEAPON_READ_FIELD(float, missileConeSoundVolumescaleAtEdge);
	WEAPON_READ_FIELD(float, missileConeSoundVolumescaleCoreSize);
	WEAPON_READ_FIELD(float, missileConeSoundPitchAtTop);
	WEAPON_READ_FIELD(float, missileConeSoundPitchAtBottom);
	WEAPON_READ_FIELD(float, missileConeSoundPitchTopSize);
	WEAPON_READ_FIELD(float, missileConeSoundPitchBottomSize);
	WEAPON_READ_FIELD(float, missileConeSoundCrossfadeTopSize);
	WEAPON_READ_FIELD(float, missileConeSoundCrossfadeBottomSize);
	WEAPON_READ_FIELD(bool, shareAmmo);
	WEAPON_READ_FIELD(bool, lockonSupported);
	WEAPON_READ_FIELD(bool, requireLockonToFire);
	WEAPON_READ_FIELD(bool, bigExplosion);
	WEAPON_READ_FIELD(bool, noAdsWhenMagEmpty);
	WEAPON_READ_FIELD(bool, avoidDropCleanup);
	WEAPON_READ_FIELD(bool, inheritsPerks);
	WEAPON_READ_FIELD(bool, crosshairColorChange);
	WEAPON_READ_FIELD(bool, rifleBullet);
	WEAPON_READ_FIELD(bool, armorPiercing);
	WEAPON_READ_FIELD(bool, boltAction);
	WEAPON_READ_FIELD(bool, aimDownSight);
	WEAPON_READ_FIELD(bool, rechamberWhileAds);
	WEAPON_READ_FIELD(bool, bBulletExplosiveDamage);
	WEAPON_READ_FIELD(bool, cookOffHold);
	WEAPON_READ_FIELD(bool, clipOnly);
	WEAPON_READ_FIELD(bool, noAmmoPickup);
	WEAPON_READ_FIELD(bool, adsFire);
	WEAPON_READ_FIELD(bool, cancelAutoHolsterWhenEmpty);
	WEAPON_READ_FIELD(bool, disableSwitchToWhenEmpty);
	WEAPON_READ_FIELD(bool, suppressAmmoReserveDisplay);
	WEAPON_READ_FIELD(bool, laserSightDuringNightvision);
	WEAPON_READ_FIELD(bool, markableViewmodel);
	WEAPON_READ_FIELD(bool, noDualWield);
	WEAPON_READ_FIELD(bool, flipKillIcon);
	WEAPON_READ_FIELD(bool, noPartialReload);
	WEAPON_READ_FIELD(bool, segmentedReload);
	WEAPON_READ_FIELD(bool, blocksProne);
	WEAPON_READ_FIELD(bool, silenced);
	WEAPON_READ_FIELD(bool, isRollingGrenade);
	WEAPON_READ_FIELD(bool, projExplosionEffectForceNormalUp);
	WEAPON_READ_FIELD(bool, projImpactExplode);
	WEAPON_READ_FIELD(bool, stickToPlayers);
	WEAPON_READ_FIELD(bool, hasDetonator);
	WEAPON_READ_FIELD(bool, disableFiring);
	WEAPON_READ_FIELD(bool, timedDetonation);
	WEAPON_READ_FIELD(bool, rotate);
	WEAPON_READ_FIELD(bool, holdButtonToThrow);
	WEAPON_READ_FIELD(bool, freezeMovementWhenFiring);
	WEAPON_READ_FIELD(bool, thermalScope);
	WEAPON_READ_FIELD(bool, altModeSameWeapon);
	WEAPON_READ_FIELD(bool, turretBarrelSpinEnabled);
	WEAPON_READ_FIELD(bool, missileConeSoundEnabled);
	WEAPON_READ_FIELD(bool, missileConeSoundPitchshiftEnabled);
	WEAPON_READ_FIELD(bool, missileConeSoundCrossfadeEnabled);
	WEAPON_READ_FIELD(bool, offhandHoldIsCancelable);

	return weapon;
}

WeaponCompleteDef* ReadWeapon(const std::string name, ZoneMemory* mem, DB_FindXAssetHeader_t finder)
{
	auto weapon = mem->Alloc<WeaponCompleteDef>();

	std::stringstream buffer;
	std::ifstream ifs;
	ifs.open(name);

	buffer << ifs.rdbuf();

	auto json = buffer.str();

	auto data = Json::parse(json.c_str());

	// base asset
	auto base = data["baseAsset"].get<std::string>();
	WeaponCompleteDef* baseAsset = nullptr;

	if (!base.empty())
	{
		baseAsset = finder(XAssetType::weapon, base.data(), 1).weapon;
		memcpy(weapon, baseAsset, sizeof WeaponCompleteDef);
	}
	else
	{
		printf("No base asset is defined for weapon %s, stuff might go wrong!\n", name.data());
	}


	memcpy(weapon, baseAsset, sizeof WeaponCompleteDef);

	weapon->WeaponDef = ReadWeaponDef(data["WeaponDef"], baseAsset, mem);

	WEAPON_READ_STRING(name);
	WEAPON_READ_STRING(displayName);

	weapon->hideTags = mem->Alloc<short>(32);
	for (int i = 0; i < 32; i++)
	{
		auto hideTag = data["hideTags"][i].get<std::string>();

		printf("hidetag: %s\n", hideTag.c_str());

		weapon->hideTags[i] = SL_AllocString(hideTag);
	}

	weapon->attachment1 = mem->Alloc<AttachmentDef*>(6);
	for (int i = 0; i < 6; i++)
	{
		auto attachment = data["attachment1"][i].get<std::string>();

		if (!attachment.empty())
		{
			weapon->attachment1[i] = IW5::DB_FindXAssetHeader(XAssetType::attachment, attachment.data(), 1).
				attachment;
		}
	}
	weapon->attachment2 = mem->Alloc<AttachmentDef*>(3);
	for (int i = 0; i < 3; i++)
	{
		auto attachment = data["attachment2"][i].get<std::string>();

		if (!attachment.empty())
		{
			weapon->attachment2[i] = IW5::DB_FindXAssetHeader(XAssetType::attachment, attachment.data(), 1).
				attachment;
		}
	}
	weapon->attachment3 = mem->Alloc<AttachmentDef*>(4);
	for (int i = 0; i < 4; i++)
	{
		auto attachment = data["attachment3"][i].get<std::string>();

		if (!attachment.empty())
		{
			weapon->attachment3[i] = IW5::DB_FindXAssetHeader(XAssetType::attachment, attachment.data(), 1).
				attachment;
		}
	}

	weapon->szXAnims = mem->Alloc<const char*>(42);
	for (int i = 0; i < 42; i++)
	{
		weapon->szXAnims[i] = mem->StrDup(data["szXAnims"][i].get<std::string>());
	}

	WEAPON_READ_FIELD(unsigned int, numAnimOverrides);
	WEAPON_READ_FIELD(unsigned int, numSoundOverrides);
	WEAPON_READ_FIELD(unsigned int, numFXOverrides);
	WEAPON_READ_FIELD(unsigned int, numReloadStateTimerOverrides);
	WEAPON_READ_FIELD(unsigned int, numNotetrackOverrides);

	if (weapon->numAnimOverrides)
	{
		weapon->animOverrides = mem->Alloc<AnimOverrideEntry>(weapon->numAnimOverrides);
		for (int i = 0; i < weapon->numAnimOverrides; i++)
		{
			weapon->animOverrides[i].altmodeAnim = mem->StrDup(
				data["animOverrides"][i]["altmodeAnim"].is_string()
				? data["animOverrides"][i]["altmodeAnim"].get<std::string>()
				: "");
			weapon->animOverrides[i].overrideAnim = mem->StrDup(
				data["animOverrides"][i]["overrideAnim"].is_string()
				? data["animOverrides"][i]["overrideAnim"].get<std::string>()
				: "");
			weapon->animOverrides[i].attachment1 = data["animOverrides"][i]["attachment1"].get<unsigned short
			>();
			weapon->animOverrides[i].attachment2 = data["animOverrides"][i]["attachment2"].get<unsigned short
			>();
			weapon->animOverrides[i].altTime = data["animOverrides"][i]["altTime"].get<int>();
			weapon->animOverrides[i].animTime = data["animOverrides"][i]["animTime"].get<int>();
			weapon->animOverrides[i].animTreeType = data["animOverrides"][i]["animTreeType"].get<unsigned int
			>();
		}
	}

	if (weapon->numSoundOverrides)
	{
		weapon->soundOverrides = mem->Alloc<SoundOverrideEntry>(weapon->numSoundOverrides);
		for (int i = 0; i < weapon->numSoundOverrides; i++)
		{
			weapon->soundOverrides[i].altmodeSound = (data["soundOverrides"][i]["altmodeSound"].is_string())
				? IW5::DB_FindXAssetHeader(
					sound,
					data["soundOverrides"][i]["altmodeSound"]
					.get<std::string>().data(), 1).sound
				: nullptr;
			weapon->soundOverrides[i].overrideSound = (data["soundOverrides"][i]["overrideSound"].is_string())
				? IW5::DB_FindXAssetHeader(
					sound,
					data["soundOverrides"][i]["overrideSound"]
					.get<std::string>().data(), 1).sound
				: nullptr;
			weapon->soundOverrides[i].attachment1 = data["soundOverrides"][i]["attachment1"].get<unsigned short
			>();
			weapon->soundOverrides[i].attachment2 = data["soundOverrides"][i]["attachment2"].get<unsigned short
			>();
			weapon->soundOverrides[i].soundType = data["soundOverrides"][i]["soundType"].get<unsigned int>();
		}
	}

	if (weapon->numFXOverrides)
	{
		weapon->fxOverrides = mem->Alloc<FXOverrideEntry>(weapon->numFXOverrides);
		for (int i = 0; i < weapon->numFXOverrides; i++)
		{
			weapon->fxOverrides[i].altmodeFX = (data["fxOverrides"][i]["altmodeFX"].is_string())
				? IW5::DB_FindXAssetHeader(
					fx, data["fxOverrides"][i]["altmodeFX"]
					.get<std::string>().data(), 1).fx
				: nullptr;
			weapon->fxOverrides[i].overrideFX = (data["fxOverrides"][i]["overrideFX"].is_string())
				? IW5::DB_FindXAssetHeader(
					fx, data["fxOverrides"][i]["overrideFX"]
					.get<std::string>().data(), 1).fx
				: nullptr;
			weapon->fxOverrides[i].attachment1 = data["fxOverrides"][i]["attachment1"].get<unsigned short>();
			weapon->fxOverrides[i].attachment2 = data["fxOverrides"][i]["attachment2"].get<unsigned short>();
			weapon->fxOverrides[i].fxType = data["fxOverrides"][i]["fxType"].get<unsigned int>();
		}
	}

	if (weapon->numReloadStateTimerOverrides)
	{
		weapon->reloadOverrides = mem->Alloc<ReloadStateTimerEntry>(weapon->numReloadStateTimerOverrides);
		for (int i = 0; i < weapon->numReloadStateTimerOverrides; i++)
		{
			try
			{
				weapon->reloadOverrides[i].attachment = data["attachment"].get<int>();
				weapon->reloadOverrides[i].reloadAddTime = data["reloadAddTime"].get<int>();
				weapon->reloadOverrides[i].reloadEmptyAddTime = data["reloadEmptyAddTime"].get<int>();
			}
			catch(std::exception ex)
			{
				weapon->numReloadStateTimerOverrides = 0;
				break;
			}
		}
	}

	if (weapon->numNotetrackOverrides)
	{
		// TODO
		weapon->numNotetrackOverrides = 0;
	}

	/*
		for (int j = 0; j < 24; j++)
		{
			data["notetrackOverrides"][i]["notetrackSoundMapKeys"][j] = SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapKeys[j]);
			data["notetrackOverrides"][i]["notetrackSoundMapValues"][j] = SL_ConvertToString(asset->notetrackOverrides[i].notetrackSoundMapValues[j]);
		}
	*/

	WEAPON_READ_FIELD(int, iAdsTransInTime);
	WEAPON_READ_FIELD(int, iAdsTransOutTime);
	WEAPON_READ_FIELD(int, iClipSize);
	WEAPON_READ_FIELD(int, impactType);
	WEAPON_READ_FIELD(int, iFireTime);
	WEAPON_READ_FIELD(int, iFireTimeAkimbo);
	WEAPON_READ_FIELD(int, dpadIconRatio);
	WEAPON_READ_FIELD(int, iPenetrateMultiplier);
	WEAPON_READ_FIELD(float, fAdsViewKickCenterSpeed);
	WEAPON_READ_FIELD(float, fHipViewKickCenterSpeed);
	WEAPON_READ_STRING(altWeaponName);
	WEAPON_READ_FIELD(unsigned int, altWeaponIndex);
	WEAPON_READ_FIELD(int, iAltRaiseTime);
	WEAPON_READ_FIELD(int, iAltRaiseTimeAkimbo);
	WEAPON_READ_ASSET(material, material, killIcon);
	WEAPON_READ_ASSET(material, material, dpadIcon);
	WEAPON_READ_FIELD(int, iDropAmmoMax);
	WEAPON_READ_FIELD(bool, motionTracker);
	WEAPON_READ_FIELD(bool, enhanced);
	WEAPON_READ_FIELD(bool, dpadIconShowsAmmo);

	return weapon;
}

LoadedSound* ReadLoadedSound(const std::string name, ZoneMemory* zonemem)
{
	auto fp = fopen(name.c_str(), "rb");
	auto result = zonemem->Alloc<LoadedSound>();


	unsigned int chunkIDBuffer;
	unsigned int chunkSize;

	fread(&chunkIDBuffer, 4, 1, fp);
	if (chunkIDBuffer != 0x46464952) // RIFF
	{
		printf("%s: Invalid RIFF Header.\n", name.c_str());
		fclose(fp);
		return nullptr;
	}

	fread(&chunkSize, 4, 1, fp);
	fread(&chunkIDBuffer, 4, 1, fp);

	if (chunkIDBuffer != 0x45564157) // WAVE
	{
		printf("%s: Invalid WAVE Header.\n", name.c_str());
		fclose(fp);
		return nullptr;
	}

	char* data;

	while (!result->sound.data && !feof(fp))
	{
		fread(&chunkIDBuffer, 4, 1, fp);
		fread(&chunkSize, 4, 1, fp);
		switch (chunkIDBuffer)
		{
		case 0x20746D66: // fmt
			if (chunkSize >= 16)
			{
				short format;
				fread(&format, 2, 1, fp);
				if (format != 1 && format != 17)
				{
					printf("%s: Invalid wave format %i.\n", name.c_str(), format);
					fclose(fp);
					return nullptr;
				}
				result->sound.info.format = format;

				short numChannels;
				fread(&numChannels, 2, 1, fp);
				result->sound.info.channels = numChannels;

				int sampleRate;
				fread(&sampleRate, 4, 1, fp);
				result->sound.info.rate = sampleRate;

				int byteRate;
				fread(&byteRate, 4, 1, fp);

				short blockAlign;
				fread(&blockAlign, 2, 1, fp);
				result->sound.info.block_size = blockAlign;

				short bitPerSample;
				fread(&bitPerSample, 2, 1, fp);
				result->sound.info.bits = bitPerSample;

				if (chunkSize > 16)
				{
					fseek(fp, chunkSize - 16, SEEK_CUR);
				}
			}
			break;

		case 0x61746164: // data
			result->sound.info.data_len = chunkSize;
			data = (char*)malloc(result->sound.info.data_len);
			fread(data, 1, result->sound.info.data_len, fp);
			result->sound.data = data;
			result->sound.info.data_ptr = data;

			result->sound.info.samples = chunkSize / (result->sound.info.bits / 8);
			result->sound.info.initial_ptr = data;
			break;

		default:
			if (chunkSize > 0)
			{
				fseek(fp, chunkSize, SEEK_CUR);
			}
			break;
		}
	}

	if (!result->sound.data)
	{
		printf("%s: Could not read sounddata.\n", name.c_str());
		fclose(fp);
		return nullptr;
	}

	fclose(fp);

	return result;
}

XAnimParts* ReadXAnimParts(const std::string& path, ZoneMemory* zonemem)
{
	AssetReader reader(zonemem);

	if (reader.open(path))
	{
		auto asset = reader.read_single<XAnimParts>();
		asset->name = reader.read_string();

		asset->tagnames = zonemem->Alloc<unsigned short>(asset->boneCount[9]);
		for (auto bone = 0; bone < asset->boneCount[9]; bone++)
		{
			asset->tagnames[bone] = SL_AllocString(reader.read_string());
		}

		if (asset->dataByte)
		{
			asset->dataByte = reader.read_array<char>();
		}
		if (asset->dataShort)
		{
			asset->dataShort = reader.read_array<short>();
		}
		if (asset->dataInt)
		{
			asset->dataInt = reader.read_array<int>();
		}
		if (asset->randomDataShort)
		{
			asset->randomDataShort = reader.read_array<short>();
		}
		if (asset->randomDataByte)
		{
			asset->randomDataByte = reader.read_array<char>();
		}
		if (asset->randomDataInt)
		{
			asset->randomDataInt = reader.read_array<int>();
		}
		if (asset->indices.data)
		{
			if (asset->framecount > 255)
			{
				asset->indices._2 = reader.read_array<std::uint16_t>();
			}
			else
			{
				asset->indices._1 = reader.read_array<char>();
			}
		}

		if (asset->notetracks)
		{
			asset->notetracks = reader.read_array<XAnimNotifyInfo>();

			for (auto i = 0; i < asset->notetrackCount; i++)
			{
				asset->notetracks[i].name = SL_AllocString(reader.read_string());
			}
		}

		if (asset->delta)
		{
			asset->delta = nullptr;
		}

		reader.close();

		return asset;
	}

	return nullptr;
}