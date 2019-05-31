<?php
/**
 * Advertising settings destination class.
 */

use ChapterThree\AppleNewsAPI\Document\AdvertisingSettings;
use ChapterThree\AppleNewsAPI\Document\Margin;
use ChapterThree\AppleNewsAPI\Document\Layouts\AdvertisingLayout;

class ApplenewsDestinationAdvertisingSettings extends ApplenewsDestinationSingleton {

  /**
   * {@inheritdoc}
   */
  public function settings() {
    return [
      'bannerType' => AdvertisingSettings::BANNER_TYPE_ANY,
      'frequency' => 0,
      'margin' => [
        'top' => 0,
        'bottom' => 0,
      ],
    ] + parent::settings();
  }

  /**
   * {@inheritdoc}
   */
  public function getConfigForm(array $values = []) {
    $form = parent::getConfigForm($values);

    $form['settings']['bannerType'] = [
      '#title' => t('Banner type'),
      '#type' => 'select',
      '#default_value' => $this->getSetting('bannerType'),
      '#required' => TRUE,
      '#options' => [
        AdvertisingSettings::BANNER_TYPE_ANY => t('Any'),
        AdvertisingSettings::BANNER_TYPE_STANDARD => t('Standard'),
        AdvertisingSettings::BANNER_TYPE_DOUBLE => t('Double height'),
        AdvertisingSettings::BANNER_TYPE_LARGE => t('Large'),
      ],
    ];

    $form['settings']['frequency'] = [
      '#title' => t('Frequency'),
      '#type' => 'select',
      '#default_value' => $this->getSetting('frequency'),
      '#required' => TRUE,
      '#options' => range(0, 10),
    ];

    $form['settings']['margin'] = [
      '#title' => t('Margin'),
      '#type' => 'fieldset',
    ];

    $margin = $this->getSetting('margin');
    $form['settings']['margin']['top'] = [
      '#title' => t('Top'),
      '#type' => 'textfield',
      '#default_value' => isset($margin['top']) ? $margin['top'] : 0,
    ];

    $form['settings']['margin']['bottom'] = [
      '#title' => t('Bottom'),
      '#type' => 'textfield',
      '#default_value' => isset($margin['bottom']) ? $margin['bottom'] : 0,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   *
   * @return AdvertisingSettings
   *   Apple News Advertisting Settings object.
   */
  public function export() {
    $margin = $this->getSetting('margin');
    $ad_layout = new AdvertisingLayout();
    $ad_layout->setMargin(new Margin((int)$margin['top'], (int)$margin['bottom']));
    $ad_settings = new AdvertisingSettings();
    $ad_settings
      ->setBannerType($this->getSetting('bannerType'))
      ->setFrequency((int)$this->getSetting('frequency'))
      ->setLayout($ad_layout);

    return $ad_settings;
  }

}