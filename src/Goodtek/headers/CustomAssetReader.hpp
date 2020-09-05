#pragma once

IW5::WeaponDef* ReadWeaponDef(Json& data, IW5::WeaponCompleteDef* baseAsset, ZoneMemory* mem);
IW5::WeaponCompleteDef* ReadWeapon(const std::string name, ZoneMemory* mem, IW5::DB_FindXAssetHeader_t finder);
IW5::LoadedSound* ReadLoadedSound(const std::string name, ZoneMemory* zonemem);
IW5::XAnimParts* ReadXAnimParts(const std::string& path, ZoneMemory* zonemem);