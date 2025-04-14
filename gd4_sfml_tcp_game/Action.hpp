//E.T d00245315
#pragma once
enum class Action
{
	kMoveLeft,
	kMoveRight,
	kMoveUp,
	kMoveDown,
	kMissileFire,
	kBulletFireUp,
	kBulletFireDown,
	kBulletFireLeft,
	kBulletFireRight,

	//ET: movement contols for second player 
	kMoveLeftP2,
	kMoveRightP2,
	kMoveUpP2,
	kMoveDownP2,
	kMissileFireP2,
	kBulletFireP2,

	kActionCount
};