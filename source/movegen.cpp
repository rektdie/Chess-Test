#include "movegen.h"

Bitboard pawnAttacks[2][64];
Bitboard knightAttacks[64];
Bitboard kingAttacks[64];


Bitboard bishopMasks[64];
Bitboard rookMasks[64];

Bitboard bishopAttacks[64][512];
Bitboard rookAttacks[64][4096];

static Bitboard notAfile = ~files[A];
static Bitboard notHfile = ~files[H];
static Bitboard notABfile = notAfile & ~files[B];
static Bitboard notHGfile = notHfile & ~files[G];

Bitboard maskPawnAttacks(int side, int square) {
	Bitboard attacks;
	Bitboard attacker = Bitboard::GetSquare(square);

	// White
	if (!side) {
		if (((attacker << noEa) & notAfile).GetBoard()) attacks |= (attacker << noEa);
		if (((attacker << noWe) & notHfile).GetBoard()) attacks |= (attacker << noWe);
	}
	// Black
	else {
		if (((attacker << soEa) & notAfile).GetBoard()) attacks |= (attacker << soEa);
		if (((attacker << soWe) & notHfile).GetBoard()) attacks |= (attacker << soWe);
	}
	return attacks;
}

Bitboard maskKnightAttacks(int square) {
	Bitboard attacks;
	Bitboard attacker = Bitboard::GetSquare(square);

	if (((attacker << noEa + north) & notAfile).GetBoard()) attacks |= (attacker << noEa + north);
	if (((attacker << noWe + north) & notHfile).GetBoard()) attacks |= (attacker << noWe + north);
	if (((attacker << noWe + west) & notHGfile).GetBoard()) attacks |= (attacker << noWe + west);
	if (((attacker << noEa + east) & notABfile).GetBoard()) attacks |= (attacker << noEa + east);

	if (((attacker << soWe + south) & notHfile).GetBoard()) attacks |= (attacker << soWe + south);
	if (((attacker << soEa + south) & notAfile).GetBoard()) attacks |= (attacker << soEa + south);
	if (((attacker << soEa + east) & notABfile).GetBoard()) attacks |= (attacker << soEa + east);
	if (((attacker << soWe + west) & notHGfile).GetBoard()) attacks |= (attacker << soWe + west);

	return attacks;
}

Bitboard maskKingAttacks(int square) {
	Bitboard attacks;
	Bitboard attacker = Bitboard::GetSquare(square);

	attacks |= (attacker << north);
	attacks |= (attacker << south);
	
	if (((attacker << west) & notHfile).GetBoard()) attacks |= (attacker << west);
	if (((attacker << soWe) & notHfile).GetBoard()) attacks |= (attacker << soWe);
	if (((attacker << soEa) & notAfile).GetBoard()) attacks |= (attacker << soEa);
	if (((attacker << east) & notAfile).GetBoard()) attacks |= (attacker << east);
	if (((attacker << noEa) & notAfile).GetBoard()) attacks |= (attacker << noEa);
	if (((attacker << noWe) & notHfile).GetBoard()) attacks |= (attacker << noWe);

	return attacks;
}

Bitboard maskBishopAttacks(int square) {
	Bitboard attacks;

	// target rank and files
	int tr = square / 8;
	int tf = square % 8;
	
	// mask relevant occupancy bits
	
	// North West
	for (int r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) {
		attacks |= (1ULL << (r * 8 + f));
	}
	// North East
	for (int r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) {
		attacks |= (1ULL << (r * 8 + f));
	}
	// South West
	for (int r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) {
		attacks |= (1ULL << (r * 8 + f));
	}
	// South East
	for (int r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) {
		attacks |= (1ULL << (r * 8 + f));
	}

	return attacks;
}

Bitboard maskRookAttacks(int square) {
	Bitboard attacks;

	// target rank and files
	int tr = square / 8;
	int tf = square % 8;
	
	// mask relevant occupancy bits

	// North
	for (int r = tr + 1; r <= 6; r++) {
		attacks |= (1ULL << (r * 8 + tf));
	}
	// South
	for (int r = tr - 1; r >= 1; r--) {
		attacks |= (1ULL << (r * 8 + tf));
	}
	// East
	for (int f = tf + 1; f <= 6; f++) {
		attacks |= (1ULL << (tr * 8 + f));
	}
	// West
	for (int f = tf - 1; f >= 1; f--) {
		attacks |= (1ULL << (tr * 8 + f));
	}

	return attacks;
}

Bitboard bishopAttacksOnTheFly(int square, Bitboard block) {
	Bitboard attacks;

	// target rank and files
	int tr = square / 8;
	int tf = square % 8;

	Bitboard currentSquare;
	
	// generate bishop attacks
	
	// North West
	for (int r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
		currentSquare = (1ULL << (r * 8 + f));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}
	// North East
	for (int r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
		currentSquare = (1ULL << (r * 8 + f));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}
	//South West
	for (int r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
		currentSquare = (1ULL << (r * 8 + f));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}
	//South East
	for (int r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
		currentSquare = (1ULL << (r * 8 + f));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}

	return attacks;
}

Bitboard rookAttacksOnTheFly(int square, Bitboard block) {
	Bitboard attacks;

	// target rank and files
	int tr = square / 8;
	int tf = square % 8;

	Bitboard currentSquare;
	
	// generate rook attacks
	
	// North
	for (int r = tr + 1; r <= 7; r++) {
		currentSquare = (1ULL << (r * 8 + tf));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}
	// South
	for (int r = tr - 1; r >= 0; r--) {
		currentSquare = (1ULL << (r * 8 + tf));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}
	// East
	for (int f = tf + 1; f <= 7; f++) {
		currentSquare = (1ULL << (tr * 8 + f));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}
	// West
	for (int f = tf - 1; f >= 0; f--) {
		currentSquare = (1ULL << (tr * 8 + f));
		attacks |= currentSquare;
		if ((currentSquare & block).GetBoard()) break;
	}

	return attacks;
}

void initLeaperAttacks() {
	for (int square = 0; square < 64; square++) {
		pawnAttacks[White][square] = maskPawnAttacks(White, square);
		pawnAttacks[Black][square] = maskPawnAttacks(Black, square);

		knightAttacks[square] = maskKnightAttacks(square);
		kingAttacks[square] = maskKingAttacks(square);
	}
}

void initSliderAttacks() {
	for (int square = 0; square < 64; square++) {
		bishopMasks[square] = maskBishopAttacks(square);
		rookMasks[square] = maskRookAttacks(square);

		for (int index = 0; index < 512; index++) {
			Bitboard occupancy = Bitboard::getOccupancy(index, bishopMasks[square]);

			int magicIndex = (occupancy.GetBoard() * bishopMagicNumbers[square])
			>> (64 - bishopRelevantBits[square]);

			bishopAttacks[square][magicIndex] = bishopAttacksOnTheFly(square, occupancy);
		}

		for (int index = 0; index < 4096; index++) {
			Bitboard occupancy = Bitboard::getOccupancy(index, rookMasks[square]);

			int magicIndex = (occupancy.GetBoard() * rookMagicNumbers[square])
			>> (64 - rookRelevantBits[square]);

			rookAttacks[square][magicIndex] = rookAttacksOnTheFly(square, occupancy);
		}
	}
}

Bitboard getBishopAttack(int square, U64 occupancy) {
	occupancy &= bishopMasks[square].GetBoard();
	occupancy *= bishopMagicNumbers[square];
	occupancy >>= (64 - bishopRelevantBits[square]);

	return bishopAttacks[square][occupancy];
}

Bitboard getRookAttack(int square, U64 occupancy) {
	occupancy &= rookMasks[square].GetBoard();
	occupancy *= rookMagicNumbers[square];
	occupancy >>= (64 - rookRelevantBits[square]);

	return rookAttacks[square][occupancy];
}

Bitboard getQueenAttack(int square, U64 occupancy) {
	U64 bishopOccupancy = occupancy;
	U64 rookOccupancy = occupancy;

	bishopOccupancy &= bishopMasks[square].GetBoard();
	bishopOccupancy *= bishopMagicNumbers[square];
	bishopOccupancy >>= (64 - bishopRelevantBits[square]);

	rookOccupancy &= rookMasks[square].GetBoard();
	rookOccupancy *= rookMagicNumbers[square];
	rookOccupancy >>= (64 - rookRelevantBits[square]);

	return rookAttacks[square][rookOccupancy] | bishopAttacks[square][bishopOccupancy];
}

Bitboard getPieceAttacks(int square, int piece, int color, U64 occupancy) {
	if (piece == Pawn) {
		return pawnAttacks[color][square];
	} else if (piece == Knight) {
		return knightAttacks[square];
	} else if (piece == King) {
		return kingAttacks[square];
	} else if (piece == Bishop) {
		return getBishopAttack(square, occupancy);
	} else if (piece == Rook) {
		return getRookAttack(square, occupancy);
	} else {
		return getQueenAttack(square, occupancy);
	}
}

// Returns the direction to target from square
static int GetDirection(int square, int target) {
	int rankDiff = (target / 8) - (square / 8);
	int fileDiff = (target % 8) - (square % 8);

	// Main directions
	if (rankDiff == 0 && fileDiff < 0) {
		return west;
	} else if (rankDiff == 0 && fileDiff > 0) {
		return east;
	} else if (rankDiff < 0 && fileDiff == 0) {
		return south;
	} else if (rankDiff > 0 && fileDiff == 0) {
		return north;
	}

	// Diagonals
	if (abs(rankDiff) == abs(fileDiff)) {
		if (rankDiff < 0 && fileDiff < 0) {
			return soWe;
		} else if (rankDiff < 0 && fileDiff > 0) {
			return soEa;
		} else if (rankDiff > 0 && fileDiff < 0) {
			return noWe;
		} else if (rankDiff > 0 && fileDiff > 0) {
			return noEa;
		}
	}

	return 0;
}

static void GenPawnMoves(Board &board, bool color) {

}

static void GenKnightMoves(Board &board, bool color) {
	Bitboard knights = board.colors[color] & board.pieces[Knight];

	while (knights.GetBoard()) {
		int square = knights.getLS1BIndex();

		Bitboard moves = knightAttacks[square] & ~board.colors[color];
		Bitboard captures = moves & board.colors[!color];
		Bitboard quietMoves = moves & ~board.colors[!color];

		while (captures.GetBoard()) {
			int targetSquare = captures.getLS1BIndex();

			board.AddMove(Move(square, targetSquare, capture));

			captures.PopBit(targetSquare);
		}

		while (quietMoves.GetBoard()) {
			int targetSquare = quietMoves.getLS1BIndex();

			board.AddMove(Move(square, targetSquare, quiet));

			quietMoves.PopBit(targetSquare);
		}

		knights.PopBit(square);
	}	
}

static void GenRookMoves(Board &board, bool color) {

}

static void GenBishopMoves(Board &board, bool color) {

}

static void GenQueenMoves(Board &board, bool color) {

}

static void GenKingMoves(Board &board, bool color) {
	
}

void GenerateMoves(Board &board) {
	bool side = board.sideToMove;
	board.moveList.clear();

	GenPawnMoves(board, side);
	GenKnightMoves(board, side);
	GenKingMoves(board, side);
	GenBishopMoves(board, side);
	GenRookMoves(board, side);
	GenQueenMoves(board, side);
}