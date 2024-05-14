#include "bitboards.h"

void Bitboard::PrintBoard() const {
	for (int rank = 7; rank >= 0; rank--) {

		std::cout << "+---+---+---+---+---+---+---+---+\n";
		std::cout << "| ";

		for (int file = 0; file < 8; file++) {
			int square = rank * 8 + file;

			if (IsSet(square)) {
				std::cout << "X | ";
			} else {
				std::cout << ". | ";
			}
		}
		std::cout << ' ' << rank + 1 << '\n';
	}

	std::cout << "+---+---+---+---+---+---+---+---+\n";
	std::cout << "  a   b   c   d   e   f   g   h\n\n";
	std::cout << "     Board: 0x" << std::hex << m_board << '\n';
}

U64 Bitboard::GetBoard() const {
	return m_board;
}

bool Bitboard::IsSet(int square) const {
	return (1ULL << square) & m_board;
}

void Bitboard::SetBit(int square) {
	m_board |= (1ULL << square);
}

void Bitboard::PopBit(int square) {
    m_board &= ~(1ULL << square);
}

int Bitboard::PopCount() const {
	return __builtin_popcountll(m_board);
}

// LS1B = least significant first bit
int Bitboard::getLS1BIndex() const {
	if (m_board) {
		// count trailing bits before LS1B
		return Bitboard((m_board & -m_board) - 1).PopCount();
	}

	return -1;
}

Bitboard Bitboard::getOccupancy(int index, Bitboard attackMask) {
	Bitboard occupancy;
	int squareCount = attackMask.PopCount();
	
	for (int count = 0; count < squareCount; count++) {
		int square = attackMask.getLS1BIndex();
		
		attackMask.PopBit(square);

		if (index & (1 << count)) {
			occupancy |= (1ULL << square);
		}
	}

	return occupancy;
}

// Operator overloading for easier comparisons
Bitboard Bitboard::operator&(const Bitboard& other) const {
	return m_board & other.m_board;
}
Bitboard Bitboard::operator|(const Bitboard& other) const {
	return m_board | other.m_board;
}
Bitboard Bitboard::operator^(const Bitboard& other) const {
	return m_board ^ other.m_board;
}
Bitboard Bitboard::operator*(const Bitboard& other) const {
	return m_board * other.m_board;
}

Bitboard Bitboard::operator&=(const Bitboard& other) {
	return m_board &= other.m_board;
}
Bitboard Bitboard::operator|=(const Bitboard& other) {
	return m_board |= other.m_board;
}
Bitboard Bitboard::operator^=(const Bitboard& other) {
	return m_board ^= other.m_board;
}

Bitboard Bitboard::operator<<(int other) {
	if (other > 0) {
		return m_board << other;
	}
	else {
		return m_board >> other * -1;
	}
}
Bitboard Bitboard::operator>>(int other) {
	if (other > 0) {
		return m_board >> other;
	}
	else {
		return m_board << other * -1;
	}
}

Bitboard Bitboard::operator~() {
	return ~(m_board);
}
