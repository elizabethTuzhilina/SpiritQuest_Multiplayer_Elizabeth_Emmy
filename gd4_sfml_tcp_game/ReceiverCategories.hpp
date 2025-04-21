//E.I D00244320, E.T d00245315
#pragma once
enum class ReceiverCategories
{
	kNone = 0,
	kScene = 1 << 0,
	kPlayer1 = 1 << 1,
	kPlayer2 = 1 << 9,
	kGhostR = 1 << 2,
	kReaperR = 1 << 3,

	kAlliedProjectile = 1 << 4,
	kEnemyProjectile = 1 << 5,
	kDeteriorate = 1 << 5,
	kPickup = 1 << 6,
	kParticleSystem = 1 << 7,
	kSoundEffect = 1 << 8,
	kNetwork = 1 << 9,

	kAircraft = kPlayer1 | kGhostR | kReaperR,
	kProjectile = kAlliedProjectile | kEnemyProjectile
};

// A message would be sent to all aircraft
//unsigned int all_aircraft = ReceiverCategories::kPlayerAircraft | ReceiverCategories::kAlliedAircraft | ReceiverCategories::kEnemyAircraft