#ifndef __COMMON_H__
#define __COMMON_H__
#include <thread>

enum class EConvexhullAlgorithm : int {
	Extreme_edges,
	Gift_wrapping,
	Quick_hull,
	Graham,
	Incremental,
	Divide_and_conquer,
};
static constexpr const char* ConvexhullAlgorithmStr = "Extreme_edges\0Gift_wrapping\0Quick_hull\0Positions\0Graham\0Incremental\0Divide_and_conquer\0\0";

// Points cloud for building a convex hull 
enum class EObjectState: int{
    PointsClouds,
    ConvexHull,
    MeshClouds,
};
static constexpr const char* ObjectStatestr = "PointsClouds\0ConvexHull\0MeshClouds\0\0";

enum class EObjectType : int {
    StandfordBunny,
    Dragon,
	Sphere,
};
static constexpr const char* ObjectTypestr = "StandfordBunny\0Dragon\0Sphere\0\0";

enum class EEmaType {
	Time,
	Step,
};

class Ema {
public:
	Ema(EEmaType type, float half_life)
	: m_type{type}, m_decay{std::pow(0.5f, 1.0f / half_life)}, m_creation_time{std::chrono::steady_clock::now()} {}

	int64_t current_progress() {
		if (m_type == EEmaType::Time) {
			auto now = std::chrono::steady_clock::now();
			return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_creation_time).count();
		} else {
			return m_last_progress + 1;
		}
	}

	void update(float val) {
		int64_t cur = current_progress();
		int64_t elapsed = cur - m_last_progress;
		m_last_progress = cur;

		float decay = std::pow(m_decay, elapsed);
		m_val = val;
		m_ema_val = decay * m_ema_val + (1.0f - decay) * val;
	}

	void set(float val) {
		m_last_progress = current_progress();
		m_val = m_ema_val = val;
	}

	float val() const {
		return m_val;
	}

	float ema_val() const {
		return m_ema_val;
	}

private:
	float m_val = 0.0f;
	float m_ema_val = 0.0f;
	EEmaType m_type;
	float m_decay;

	int64_t m_last_progress = 0;
	std::chrono::time_point<std::chrono::steady_clock> m_creation_time;
};

#endif