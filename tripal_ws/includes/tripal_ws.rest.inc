<?php

function tripal_ws_rest() {

  global $base_url;

  // Set some initial variables.
  $response = array();
  $status = 'success';
  $version = 'v0.1';
  $message = '';
  $api_url = $base_url . '/ws/bio-data/' . $version;
  $page_limit = 25;
  $pager_id = 0;

  $response['@context'] =  array(
    'schema' => 'http://schema.org/',
    'foaf' => 'http://xmlns.com/foaf/0.1/',
    'dc' => 'http://purl.org/dc/elements/1.1/',
  );
  $response['@id'] = $api_url;

  $namespace   = arg(3) ? arg(3) : '';
  $bundle_name = arg(4) ? arg(4) : '';

  // Lump everything ito a try block so that if there is a problem we can
  // throw an error and have that returned in the response.
  try {
    if (!$namespace) {
      tripal_ws_get_content_types($api_url, $response);
    }
    if ($namespace and !$bundle_name) {
      tripal_ws_get_content_type($api_url, $response, $namespace);
    }

  }
  catch (Exception $e) {
    watchdog('tripal_ws', $e->getMessage(), array(), WATCHDOG_ERROR);
    $message = $e->getMessage();
    $status = 'error';
  }

  // The responses follow a similar format as the AGAVE API with a
  // status, message, version and all data in the 'result' object.
/*   $response['status']  = $status;
  $response['message'] = $message;
  $response['api_version'] = $version;
  $response['source'] = array(
    'site_name' => variable_get('site_name', 'Unspecified'),
    'site_url' => $base_url,
    'site_slogan' => variable_get('site_slogan', 'Unspecified'),
    'site_email' =>  variable_get('site_mail', 'Unspecified'),
  ); */
  print drupal_json_output($response);
}
/**
 *
 * @param unknown $api_url
 * @param unknown $response
 */
function tripal_ws_get_content_types($api_url, &$response) {
  $response['@type'] = 'ItemList';
  $response['schema:itemListOrder'] = 'ItemListOrderAscending';
  $response['schema:numberOfItems'] = 0;
  // Get the list of published terms (these are the bundle IDs)
  $bundles = db_select('tripal_bundle', 'tb')
    ->fields('tb')
    ->orderBy('tb.label', 'ASC')
    ->execute();
  $terms = array();
  while ($bundle = $bundles->fetchObject()) {
    $entity =  entity_load('TripalTerm', array('id' => $bundle->term_id));
    $terms[$bundle->label] = reset($entity);
  }

  $i = 0;
  foreach ($terms as $name => $term) {
    $vocab = $term->vocab;
    if (!array_key_exists($vocab->namespace, $response['@context'])) {
      // If there is no URL prefix then use this API's vocabulary API
      if ($term->urlprefix) {
        $response['@context'][$vocab->namespace] = $term->urlprefix;
      }
      else {
        $response['@context'][$vocab->namespace] = $api_url . '/TODO/';
      }
    }
    $response['schema:itemListElement'][] = array(
      '@type' => 'schema:ListItem',
      'schema:position' => $i + 1,
      'schema:item' => array(
        '@id' => $api_url . '/' . $term->vocab->namespace .'/' . $term->name,
        '@type' => $vocab->namespace . ':' . $term->accession,
        'foaf:name' => $term->name,
        'dc:description' => $term->definition,
      ),
    );
    $i++;
  }
  $response['schema:numberOfItems'] = $i;
}

/**
 *
 * @param unknown $api_url
 * @param unknown $response
 * @param unknown $vocab
 * @param unknown $name
 */
function tripal_ws_get_content_type($api_url, &$response, $vocab, $name) {

}
