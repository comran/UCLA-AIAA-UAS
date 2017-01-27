#include "util/license.h"

namespace cs31_project_2 {
namespace {
struct TaxBracket {
  double lower_limit;
  double upper_limit;
  double rate;
};

static constexpr TaxBracket kLowerBaseTaxRate{0, 20, 18.1 / 100};
static constexpr TaxBracket kGenericCountryTaxRate{20, 50, 20.3 / 100};
static constexpr TaxBracket kUAEAndTurkeyTaxRate{20, 50, 21.7 / 100};
static constexpr TaxBracket kUpperBaseTaxRate{
    50, ::std::numeric_limits<double>::infinity(), 23.0 / 100};

}  // namespace

class LicenseToBill {
 public:
  LicenseToBill();

 private:
  double CalculateRevenue(double millions);
  double DoTaxForBracket(TaxBracket bracket, double revenue);
  ::std::string ParseCash(double revenue);

  ::std::string id_, country_;
  double revenue_in_millions_;
};

// Handles the high level I/O of the program command line interface that gathers
// sanitized input from the user, performs a calculation on the data, and
// sends the calculated revenue to the user.
LicenseToBill::LicenseToBill() {
  ::std::cout << "Identification: ";
  ::std::getline(::std::cin, id_);
  if (id_ == "") {
    ::std::cout << "---\nYou must enter a property identification." << ::std::endl;
    return;
  }

  ::std::cout << "Expected revenue (in millions): ";
  ::std::cin >> revenue_in_millions_;
  ::std::cin.ignore(10000, '\n');
  if (revenue_in_millions_ < 0) {
    ::std::cout << "---\nThe expected revenue must be nonnegative." << ::std::endl;
    return;
  }

  ::std::cout << "Country: ";
  ::std::getline(::std::cin, country_);
  if (country_ == "") {
    ::std::cout << "---\nYou must enter a country." << ::std::endl;
    return;
  }

  ::std::cout << "---\nThe license fee for " << id_ << " is "
              << ParseCash(CalculateRevenue(revenue_in_millions_)) << "."
              << ::std::endl;
}

// Sums together the tax amount for each tax bracket.
double LicenseToBill::CalculateRevenue(double revenue) {
  // Default to kGenericCountryTaxRate, unless a special case country is
  // entered.
  TaxBracket country_tax_bracket = kGenericCountryTaxRate;
  if (country_ == "UAE" || country_ == "Turkey") {
    country_tax_bracket = kUAEAndTurkeyTaxRate;
  }

  return DoTaxForBracket(kLowerBaseTaxRate, revenue) +
         DoTaxForBracket(country_tax_bracket, revenue) +
         DoTaxForBracket(kUpperBaseTaxRate, revenue);
}

double LicenseToBill::DoTaxForBracket(TaxBracket bracket, double revenue) {
  if (revenue < bracket.upper_limit && revenue > bracket.lower_limit) {
    // This is the highest tax bracket that will be used, so return the
    // appropriate amount based on the revenue.
    return (revenue - bracket.lower_limit) * bracket.rate;
  } else if (revenue > bracket.lower_limit) {
    // Tax bracket is filled, so return the maximum tax amount for that bracket.
    return (bracket.upper_limit - bracket.lower_limit) * bracket.rate;
  }

  // Revenue does not fall into this tax bracket, so return no tax.
  return 0;
}

::std::string LicenseToBill::ParseCash(double cash_in_millions) {
  // Return a string containing the cash amount formatted according to the
  // project specs.

  ::std::stringstream parsed_cash;
  parsed_cash << "$" << ::std::setprecision(3)
              << ::std::setiosflags(std::ios_base::fixed) << cash_in_millions
              << " million";
  return parsed_cash.str();
}

}  // namespace cs31_project_2

int main(int /*argc*/, char * /*argv*/ []) {
  ::cs31_project_2::LicenseToBill license_to_bill;

  return 0;
}
