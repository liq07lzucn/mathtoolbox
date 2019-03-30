#ifndef STRONG_WOLFE_CONDITIONS_LINE_SEARCH_HPP
#define STRONG_WOLFE_CONDITIONS_LINE_SEARCH_HPP

#include <cassert>
#include <functional>
#include <Eigen/Core>

namespace mathtoolbox
{
    namespace optimization
    {
        // Algorithm 3.2: Line Search Algorithm
        inline double RunStrongWolfeConditionsLineSearch(const std::function<double(const Eigen::VectorXd&)>& f,
                                                         const std::function<Eigen::VectorXd(const Eigen::VectorXd&)>& g,
                                                         const Eigen::VectorXd& x,
                                                         const Eigen::VectorXd& p,
                                                         const double alpha_init,
                                                         const double alpha_max,
                                                         const double c_1 = 1e-04,
                                                         const double c_2 = 0.9)
        {
            auto phi = [&](const double alpha) { return f(x + alpha * p); };
            auto phi_grad = [&](const double alpha) { return static_cast<double>(g(x + alpha * p).transpose() * p); };

            const double phi_zero = phi(0.0);
            const double phi_grad_zero = phi_grad(0.0);

            double alpha_prev = 0.0;
            double alpha = alpha_init;
            double phi_alpha_prev = phi_zero;

            bool is_first = true;

            // Algorithm 3.3
            auto zoom = [&](double alpha_l, double alpha_h)
            {
                constexpr unsigned int max_num_iterations = 50;
                for (unsigned int i = 0; i < max_num_iterations; ++ i)
                {
                    const double alpha_j = 0.5 * (alpha_l + alpha_h); // TODO: Use a better strategy
                    const double phi_alpha_j = phi(alpha_j);

                    if (phi_alpha_j > phi_zero + c_1 * alpha_j * phi_grad_zero || phi_alpha_j >= phi(alpha_l))
                    {
                        alpha_h = alpha_j;
                    }
                    else
                    {
                        const double phi_grad_alpha_j = phi_grad(alpha_j);
                        if (std::abs(phi_grad_alpha_j) <= - c_2 * phi_grad_zero)
                        {
                            return alpha_j;
                        }
                        if (phi_grad_alpha_j * (alpha_h - alpha_l) >= 0.0)
                        {
                            alpha_h = alpha_l;
                        }
                        alpha_l = alpha_j;
                    }
                }
                throw std::runtime_error("Failed to perform the line search.");
            };

            while (true)
            {
                const double phi_alpha = phi(alpha);

                if (phi_alpha > phi_zero + c_1 * alpha * phi_grad_zero || (!is_first && (phi_alpha >= phi_alpha_prev)))
                {
                    return zoom(alpha_prev, alpha);
                }

                const double phi_grad_alpha = phi_grad(alpha);

                if (std::abs(phi_grad_alpha) <= - c_2 * phi_grad_zero)
                {
                    return alpha;
                }

                if (phi_grad_alpha >= 0.0)
                {
                    return zoom(alpha, alpha_prev);
                }

                alpha_prev = alpha;
                phi_alpha_prev = phi_alpha;

                alpha = 0.5 * (alpha + alpha_max); // TODO: Use a better strategy
            }

            assert(false);

            return alpha_init;
        }
    }
}

#endif /* STRONG_WOLFE_CONDITIONS_LINE_SEARCH_HPP */
