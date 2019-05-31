<div class="card">

  <div class="card-header d-flex justify-content-between p-0 py-3 px-3 align-items-center">
    <div class="col px-1">
      <a class="btn <?php print (isset($_GET['recurring']) && $_GET['recurring'] == 1 ? 'btn-secondary' : 'btn-primary'); ?> border w-100 change-donation-type" data-donation-type="once" href="#">Give Once</a>
    </div>
    <div class="col px-1">
      <a class="btn <?php print (isset($_GET['recurring']) && $_GET['recurring'] == 1 ? 'btn-primary' : 'btn-secondary'); ?> w-100 change-donation-type" data-donation-type="monthly" href="#">Monthly</a>
    </div>
  </div>

  <div class="card-body">

    <div class="row">
      <div class="col-12">
        <div id="error_explanation alert alert-danger"></div>
      </div>
    </div>

    <div class="row my-2 suggested-amounts">
      <div class="d-none d-sm-block col-3 px-1 my-1 my-sm-0">
        <a class="btn btn-secondary w-100 change-amount" data-amount="15" href="#">$15</a>
      </div>
      <div class="d-none d-sm-block col-3 px-1 my-1 my-sm-0">
        <a class="btn btn-secondary w-100 change-amount" data-amount="27" href="#">$27</a>
      </div>
      <div class="d-none d-sm-block col-3 px-1 my-1 my-sm-0">
        <a class="btn btn-secondary w-100 change-amount" data-amount="75" href="#">$75</a>
      </div>
      <div class="d-none d-sm-block col-3 px-1 my-1 my-sm-0">
        <a class="btn btn-secondary w-100 change-amount" data-amount="75" href="#">$75</a>
      </div>
    </div>
    <div class="row my-2 suggested-amounts">
      <div class="d-none d-sm-block col-3 px-1 my-1 my-sm-0">
        <a class="btn btn-secondary w-100 change-amount" data-amount="250" href="#">$250</a>
      </div>
      <div class="d-none d-sm-block col-3 px-1 my-1 my-sm-0">
        <a class="btn btn-secondary w-100 change-amount" data-amount="75" href="#">$75</a>
      </div>
      <div class="d-none d-sm-block col-3 px-1 my-1 my-sm-0">
        <a class="btn btn-secondary w-100 change-amount" data-amount="75" href="#">$75</a>
      </div>
      <div id="other-amount" class="col-xs-12 px-1 col-sm-3 my-sm-0">
        <div class="input-group mb-2">
          <div class="input-group-prepend">
            <div class="input-group-text">$</div>
          </div>
          <input type="text" class="form-control" id="customAmount" placeholder="0.00">
        </div>
      </div>
    </div>
    <div class="row mt-3">
      <div class="col-12 px-1">
        <a class="btn btn-primary w-100" href="#" id="donateButton" data-final-amount="<?php print (isset($_GET['value']) && is_numeric($_GET['value']) ? $_GET['value'] : ''); ?>" data-donation-type="<?php print (isset($_GET['recurring']) && $_GET['recurring'] == 1 ? 'monthly' : 'once'); ?>">
          <span id="donateType">Donate</span> <span id="donateAmountDisplay"><?php print (isset($_GET['value']) && is_numeric($_GET['value']) ? '$' . $_GET['value'] : ''); ?></span> by credit/debit card </a>
      </div>
    </div>
  </div>
</div>
<?php print drupal_render_children($form); ?>
