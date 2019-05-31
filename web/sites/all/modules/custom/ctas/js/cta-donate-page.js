(function ($) {

  var amounts = [];

  Drupal.behaviors.superHeader = {
    attach: function (context, settings) {

      var handler = StripeCheckout.configure({
        key: settings.stripe_pub_key,
        locale: 'auto',
        name: 'Common Dreams',
        description: 'Donation',
        zipCode: true,
        billingAddress: true,
        panelLabel: 'Donate',
        allowRememberMe: false,
        image: '/sites/all/themes/omega_dreams/images/commondreams-earth-logo.png',
        token: function(token) {
          $('input#stripeToken').val(JSON.stringify(token));
          $('input#stripeDonationType').val($("#donateButton").data('donation-type'));
          $('#ctas-donate-form').submit();
        }
      });

      // Change amount
      $(".change-amount").on('click', function(e) {
        e.preventDefault();
        $("#customAmount").val("");
        $(".change-amount").removeClass('btn-primary').addClass('btn-secondary');
        $(this).removeClass('btn-secondary').addClass('btn-primary');
        $("#donateButton").data('final-amount', $(this).data('amount'));
        $("#donateAmountDisplay").text("$" + $(this).data('amount'));
      });

      // Other amount.
      $("#customAmount").on('keyup', function(e) {
        $(".change-amount").removeClass('btn-primary').addClass('btn-secondary');
        $("#donateButton").data('final-amount', $(this).val());
        $("#donateAmountDisplay").text("$" + $(this).val());
      });

      // Change subscription.
      $(".change-donation-type").on("click", function(e) {
        $("#donateButton").data('donation-type', $(this).data("donation-type"));
        $(".change-donation-type").removeClass('btn-primary').addClass('btn-secondary');
        $(this).removeClass('btn-secondary').addClass('btn-primary');

        var type = $(this).data("donation-type");
        if (type === "once") {
          $("#donateType").text("Give Once");
        }
        else {
          $("#donateType").text("Donate Monthly");
        }
        updateForm();
        e.preventDefault();
      });

      $('#donateButton').on('click', function(e) {
        e.preventDefault();
        $('#error_explanation').html('');
        var amount = $(this).data('final-amount');
        var donationType = $(this).data('donation-type');
        amount = amount.toString().replace(/\$/g, '').replace(/\,/g, '')
        amount = parseFloat(amount);
        if (isNaN(amount)) {
          $('#error_explanation').html('<p>Please enter a valid amount in USD ($).</p>');
        }
        // else if (amount < 5.00) {
        //   $('#error_explanation').html('<p>Donation amount must be at least $5.</p>');
        // }
        else {
          amount = amount * 100; // Needs to be an integer!

          $('input#stripeAmount').val(amount);

          handler.open({
            amount: Math.round(amount),
            description: $("#donateButton").data('donation-type') === "monthly" ? "Monthly Donation" : "Donation",
            donationType: donationType
          });
        }
      });


      // Change custom amount for mobile.
      updateForm();
      window.onresize = updateForm;

    }
  };

  function updateForm() {
    updateCustomAmount();
    updateSuggestedAmounts();
  }

  // Set the Custom Amount on Mobile.
  function updateCustomAmount() {
    if (window.innerWidth < 360) {
      $("#customAmount").val(35.00);
    }
    else {
      $("#customAmount").val("");
    }
  }


  // Change the Suggested Amounts.
  function updateSuggestedAmounts() {
    var type = $("#donateButton").data('donation-type');
    switch(type) {
      case "once":
        amounts = [15,27,75,100,250,300,400];
        break;
      case "monthly":
        amounts = [7,15,25,35,50,75,100];
        break;
    }

    var count = 0;
    $(".change-amount").each(function() {
      var html = "";
      if (type === "monthly") {
        html = "$" + amounts[count] + "/mo";
      }
      else {
        html = "$" + amounts[count];
      }
      $(this).html(html);
      $(this).data("amount", amounts[count]);
      count++;
    });
  }



})(jQuery);
